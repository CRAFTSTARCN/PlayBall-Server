/*
    NetServer.cpp Created 2021/5/27
    By: Ag2S
*/

#include "NetServer.h"
#include "StrTool.h"

#include <sstream>

NetServer::NetServer( int cp_num, int vp_num, int tp_num, int sender_num) : 
    certification_processor(cp_num),
    verification_processor(vp_num),
    task_processor(tp_num),
    main_thread(1),sending_thread(sender_num),
    resource_reclamation_thread(1)
{
    cur_user_id.store(0);
    is_open.store(false);
    set_for_server = nullptr;
    set_for_users = nullptr;
    server_socket = nullptr;
    listener = nullptr;
    user_listener = nullptr;
    filter = nullptr;
}

NetServer::~NetServer() {
    shutwdon();
}

int NetServer::start(ServerListenerInterface* __listener, UserListenerInterface* __u_listener, 
                     InterceptorInterface* __filter, Uint16 port) 
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
    filter = filter;
    buffer = new char[SERVER_BUFF_SIZE + 1];

    is_open.store(1);
    accept_new.store(1);

    main_thread.pushMembTask(mainLoop,this);
    return OK;
}

void NetServer::stopAccept() {
    if(!accept_new.load());

    accept_new.store(false);

    return;
}

void NetServer::shutwdon() {
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
            }
            user_access_lock.ExclusiveLock();
            change_status.store(true);
        }
    }
}

void NetServer::Reading() {
    user_access_lock.SharedLock();
    if(change_status.load()) {
        if(set_for_users) SDLNet_FreeSocketSet(set_for_users);
        set_for_users = SDLNet_AllocSocketSet(users.size());
        for(auto& u : users) SDLNet_TCP_AddSocket(set_for_users,u.second->socket);
        change_status.store(false);
    }
    resetBuff();
    std::vector<unsigned int> removed_users;
    int active_socket = SDLNet_CheckSockets(set_for_users,0);
    for(UserIter u_itrer = users.begin(); u_itrer != users.end() && active_socket; ) {
        if(SDLNet_SocketReady(u_itrer->second->socket)) {
            active_socket--;
            int got_len = 0;
            if( (got_len = SDLNet_TCP_Recv(u_itrer->second->socket,buffer,SERVER_BUFF_SIZE)) > 0) {
                if(u_itrer->second->user_status == valid) {
                    task_processor.pushMembTask(
                    messageProcessor,
                    this,
                    std::move(std::string(buffer,got_len)),
                    u_itrer->second->id);
                } else if(u_itrer->second->user_status == certified) {
                    verification_processor.pushMembTask(
                        messageProcessor,
                        this,
                        std::move(std::string(buffer,got_len)),
                        u_itrer->second->id);
                } else {
                    certification_processor.pushMembTask(Certifier,
                    this,
                    std::move(std::string(buffer,got_len)),
                    u_itrer->second->id);
                }
                resetBuff();
                u_itrer++;
            } else {
                if(u_itrer->second->user_status == certified || u_itrer->second->user_status == valid) {
                    user_listener->UserQuit(u_itrer->second->id);
                }
                removed_users.push_back(u_itrer->second->id);
                change_status.store(true);
            }
        } else {
            u_itrer++;
        }
    }
    user_access_lock.SharedUnlock();
    if(!removed_users.empty()) resource_reclamation_thread.pushMembTask(rmUsers,this,std::move(removed_users));
}

void NetServer::mainLoop() {
    change_status.store(true);
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
    if(filter->banned(user_socket)) return;
    cur_user_id++;
    UserAgent *n_user = new UserAgent(cur_user_id.load(),user_socket);
    users.emplace(cur_user_id.load(),n_user);
}

void NetServer::rmUsers(const std::vector<unsigned int>& removed_users) {
    user_access_lock.ExclusiveLock();
    for(auto id : removed_users) {
        UserIter it = users.find(id);
        if(it != users.end()) {
            if(it->second->user_status != uncertified) {
                resource_reclamation_thread.pushMembTask(rmBuffer,this,id);
            }
            delete it->second;
            users.erase(it);
        }
    }
    user_access_lock.ExclusiveUnlock();
}


std::map<std::string,std::string> NetServer::headAnalyzer(const std::string& message) {
    std::map<std::string,std::string> ept,head_info;
    int beg = message.find("<head>"), end = message.find("</head>");
    if(end == -1 || end == -1) return ept;
    std::string cur_label;
    std::string cur_content;
    std::string temp;
    bool in_angle_brackets;

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

void NetServer::Certifier(const std::string& msg, unsigned int id) {
    auto head_info =  headAnalyzer(msg);
    if(head_info["id"] != std::to_string(id)) return;
    if(head_info["version"] != std::to_string(PROTOCOL_VERSION)) {
        sending_thread.pushMembTask(sendStr,this,std::string(""),id); // tobe done
        return;
    }
    if(head_info["protocolType"] == "certification") {
        user_access_lock.SharedLock();
        auto user = users.find(id);
        if(user == users.end()) return;
        user->second->user_status = certified;
        user_access_lock.SharedUnlock();

        UserBuffer* n_buffer = new UserBuffer;
        buffer_access_lock.ExclusiveLock();
        user_buffer.emplace(cur_user_id.load(),n_buffer);
        buffer_access_lock.ExclusiveUnlock();
        user_listener->UserCertified(id);
    }
    return;
}