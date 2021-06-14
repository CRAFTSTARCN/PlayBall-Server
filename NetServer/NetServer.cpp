/*
    NetServer.cpp Created 2021/5/27
    By: Ag2S
*/

#include "ServiceInfo.h"
#include "NetServer.h"
#include "StrTool.h"
#include "ACKMessage.h"
#include "iostream"

#include <sstream>

NetServer::NetServer( int cp_num, int vp_num, int tp_num, int sender_num) : 
    certification_processor(cp_num),
    verification_processor(vp_num),
    task_processor(tp_num),
    main_thread(1),sending_thread(sender_num),
    resource_reclamation_thread(1)
{
    cur_user_id = 0;
    is_open.store(false);
    set_for_server = nullptr;
    set_for_users = nullptr;
    server_socket = nullptr;
    listener = nullptr;
    user_listener = nullptr;
}

NetServer::~NetServer() {
    shutwdon();
}

int NetServer::start(ServerListenerInterface* __listener, UserListenerInterface* __u_listener, 
                     Uint16 port) 
{
    if(is_open.load()) return SERVER_HAS_BEEN_OPEN;

    int ip_status = SDLNet_ResolveHost(&server_ip_port,nullptr,port);
    if(ip_status!=0) return HOST_RESOLVE_FAIL;

    server_socket = SDLNet_TCP_Open(&server_ip_port);
    if(!server_socket) return OPEN_TCPSOCKET_FAIL;

    set_for_server = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(set_for_server,server_socket);

    listener = __listener;
    user_listener = __u_listener;
    buffer = new char[SERVER_BUFF_SIZE + 1];

    is_open.store(1);
    accept_new.store(1);
    is_verifying.store(1);
    is_certifying.store(1);

    main_thread.pushMembTask(mainLoop,this);
    std::cout<<"NetServer start"<<std::endl;
    return OK;
}


void NetServer::shutwdon() {
    if(!is_open.load()) return;
    is_open.store(false);
    accept_new.store(false);
    is_verifying.store(false);
    
    user_access_lock.ExclusiveLock();
    for(auto user_it = users.begin(); user_it != users.end();) {
        delete user_it->second;
        user_it = users.erase(user_it);
    }
    resource_reclamation_thread.forceShutdown();
    certification_processor.forceShutdown();
    verification_processor.deactivate();
    task_processor.deactivate();
    sending_thread.deactivate();
    main_thread.deactivate();
    delete buffer;

    return;
}

void NetServer::Listening() {
    if(accept_new.load()) {
        TCPsocket n_user = nullptr;
        SDLNet_CheckSockets(set_for_server,0);
        if(SDLNet_SocketReady(server_socket)) {
            user_access_lock.ExclusiveLock();
            for(int i=0; i < MAX_USER_IN_ONCE && (n_user = SDLNet_TCP_Accept(server_socket)) != nullptr; ++i ) {
                appendUser(n_user);
                std::cout<<"New user arrive"<<std::endl;
            }
            user_access_lock.ExclusiveUnlock();
            change_status = true;
        }
    }
}

void NetServer::Reading() {
    user_access_lock.SharedLock();
    if(change_status) {
        if(set_for_users) SDLNet_FreeSocketSet(set_for_users);
        set_for_users = SDLNet_AllocSocketSet(users.size());
        for(auto& u : users) SDLNet_TCP_AddSocket(set_for_users,u.second->socket);
        change_status = false;
    }
    resetBuff();
    std::vector<unsigned int> removed_users;
    int active_socket = SDLNet_CheckSockets(set_for_users,0);
    for(UserIter u_itrer = users.begin(); u_itrer != users.end() && active_socket; ) {
        if(SDLNet_SocketReady(u_itrer->second->socket)) {
            active_socket--;
            int got_len = 0;
            if( (got_len = SDLNet_TCP_Recv(u_itrer->second->socket,buffer,SERVER_BUFF_SIZE)) > 0) {
                if(u_itrer->second->user_status == uncertified && is_certifying.load()) {
                    certification_processor.pushMembTask(
                    Certifier,
                    this,
                    std::string(buffer,got_len),
                    u_itrer->second->id);
                } else {
                    messageProcessor(std::string(buffer,got_len),u_itrer->second);
                }
                resetBuff();
                u_itrer++;
            } else {
                if(u_itrer->second->user_status != uncertified) {
                    user_listener->UserQuit(u_itrer->second->id);
                }
                removed_users.push_back(u_itrer->second->id);
                change_status = true;
            }
        } else {
            u_itrer++;
        }
    }
    user_access_lock.SharedUnlock();
    if(!removed_users.empty()) resource_reclamation_thread.pushMembTask(rmUsers,this,std::move(removed_users));
}

void NetServer::mainLoop() {
    change_status = true;
    while(is_open.load()) {
        Listening();
        Reading();
    }
}

inline void NetServer::resetBuff() {
    for(int i=0; i<=SERVER_BUFF_SIZE; ++i) {
        buffer[i] = 0;
    }
}

void NetServer::appendUser(TCPsocket user_socket) {
    cur_user_id++;
    UserAgent *n_user = new UserAgent(cur_user_id,user_socket);
    users.emplace(cur_user_id,n_user);
}

void NetServer::rmUsers(const std::vector<unsigned int>& removed_users) {
    user_access_lock.ExclusiveLock();
    for(auto id : removed_users) {
        UserIter it = users.find(id);
        if(it != users.end()) {
            delete it->second;
            users.erase(it);
        }
    }
    user_access_lock.ExclusiveUnlock();
}

std::map<std::string,std::string> NetServer::headAnalyzer(const std::string& message) {
    std::map<std::string,std::string> ept,head_info;
    int beg = message.find("<head>"), end = message.find("</head>");
    if(beg == -1 || end == -1 || end < beg) return ept;
    std::string cur_label;
    std::string cur_content;
    std::string temp;
    bool in_angle_brackets = false;

    for(int i = beg + 6; i != end; ++i) {
        char c = message[i];
        switch (c) {
            case '<': 
                if(in_angle_brackets) return ept;
                in_angle_brackets = true;
                cur_content = temp;
                temp.clear();
                break;

            case '>':
                if(!in_angle_brackets || temp.empty()) return ept;
                if(cur_label.empty()) {
                    cur_label = temp;
                    temp.clear();
                } else {
                    if(StrTool::eqLabel(cur_label,temp)) {
                        head_info.emplace(cur_label,cur_content);
                        cur_content.clear();
                        cur_label.clear();
                        temp.clear();
                    } else return ept;
                }
                in_angle_brackets = false;
                break;

            default:
                if(in_angle_brackets) {
                    if( c == '/' ) {
                        if((cur_label.empty() || !temp.empty())) return ept;
                    } else if(!StrTool::validLabelContent(c)) return ept;
                }
                if(c == ' ' || c == '\n' || c == '\t') continue;
                temp.push_back(c);
                break;
        }
    }
    if(!cur_label.empty() || in_angle_brackets) return ept;
    return head_info;
}


std::vector<std::string>  NetServer::cut(const std::string& str, int& vis) {
    std::vector<std::string> msgs;
    int st = str.find("<msg>"),ed;
    if(st == -1) {
        vis = 1;
        msgs.push_back(str);
        return msgs;
    }
    if(st != 0) {
        msgs.push_back(str.substr(0,st));
        vis = 1;
    }
    ed = str.find("</msg>",st + 5);
    while(st != -1) {
        ed = str.find("</msg>",st);
        if(ed != -1) msgs.push_back(str.substr(st,ed+6-st));
        else break;
        st = str.find("<msg>",ed+1);
    }
    if(st < str.length()) { 
        msgs.push_back(str.substr(st));
        vis += 2;
    }
    return msgs;
}

void NetServer::Certifier(const std::string& msg, unsigned int id) {
    auto head_info =  headAnalyzer(msg);
    if(head_info["id"] != "-1") return;
    if(head_info["version"] != std::to_string(PROTOCOL_VERSION)) {
        sending_thread.pushMembTask(sendStr,this,std::string(""),id); // tobe done
        return;
    }
    if(head_info["protocolType"] == "certification") {
      
        user_access_lock.SharedLock();
        auto user = users.find(id);
        if(user == users.end()) {
            user_access_lock.SharedUnlock();
            return;
        }
        user->second->buffer = new std::string;
        user->second->user_status = certified;
        std::string ack = ACKMessage::ACK(id);
        SDLNet_TCP_Send(user->second->socket,ack.c_str(),ack.length());
        user_access_lock.SharedUnlock();

        user_listener->UserCertified(id);
    }
    return;
}

void NetServer::messageProcessor(const std::string& msg, UserAgent* user) {
    if(!is_verifying.load() && user->user_status == certified) return; 
    int vis = 0;
    auto msgs = cut(msg,vis);
    int valids = 0, valide = msgs.size();
    if(vis) {
        if(vis & 1) {
            if(!user->buffer->empty()) {
                msgs[0] = msgs[0] + *(user->buffer);
                if(msgs[0].rfind("</msg>") == -1) {
                    if(msgs[0].length() < MAX_MESSAGE_LEN) {
                        *user->buffer = msgs[0];
                    }
                    ++valids;
                }
            }
        } else if(vis >= 2) {
            *(user->buffer) = msgs[--valide];
        }
    }
    for(int i=valids; i<valide; ++i) {

        auto head_info = headAnalyzer(msgs[i]);
        if(head_info["id"] != std::to_string(user->id)) {
            //send id not fit
            continue;
        }
        if(head_info["version"] != std::to_string(PROTOCOL_VERSION)){
            //send version not fit
            continue;
        }
        int begc = msgs[i].find("<content>"), edc = msgs[i].find("</content>");
        if(begc == -1 || edc == -1 || edc < begc) continue;
    
        std::string content = msgs[i].substr(begc+9,edc-begc-9);
        if(user->user_status == valid) task_processor.pushMembTask(ServerListenerInterface::onEmitMessage,
                listener,user->id,PROTOCOL_VERSION,
                std::move(head_info["protocolType"]),std::move(content));
        else verification_processor.pushMembTask(ServerListenerInterface::onEmitMessage,
                listener,user->id,PROTOCOL_VERSION,
                std::move(head_info["protocolType"]),std::move(content));
    }
}

void NetServer::sendMsg(AbstractMessage* msg) {
    std::string text = msg->toText();
    sendStr(text,msg->to_id);
    delete msg;
}

void NetServer::sendStr(const std::string& str, int id) {
    user_access_lock.SharedLock();
    auto user_it = users.find(id);
    if(user_it != users.end()) {
        SDLNet_TCP_Send(user_it->second->socket,str.c_str(),str.length());
    }
    user_access_lock.SharedUnlock();
}

void NetServer::stopAccept() {
    if(accept_new.load())
        accept_new.store(false);
    return;
}

void NetServer::stopCertify() {
    if(!is_certifying.load()) return;
    is_certifying.store(false);
    certification_processor.deactivate();
    std::vector<unsigned int> deleted;
    user_access_lock.SharedLock();
    for(auto &user_it : users) {
        if(user_it.second->user_status == uncertified) deleted.push_back(user_it.first);
    }
    user_access_lock.SharedUnlock();
    rmUsers(deleted);
}

void NetServer::stopVerify() {
    if(!is_verifying.load()) return;
    is_verifying.store(false);
    verification_processor.deactivate();
}

void NetServer::setValid(unsigned int id) {
    user_access_lock.SharedLock();
    auto user_it = users.find(id);
    if(user_it == users.end()) {
        user_access_lock.SharedUnlock();
        return;
    }
    user_it->second->user_status = valid;
    user_access_lock.SharedUnlock();
}

void NetServer::pushMessage(AbstractMessage* msg) {
    sending_thread.pushMembTask(sendMsg,this,msg);
}
