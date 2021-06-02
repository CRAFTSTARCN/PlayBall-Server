/*
    NetServer.cpp Created 2021/5/27
    By: Ag2S
*/

#include "NetServer.h"

NetServer::NetServer( int cp_num, int vp_num, int tp_num, int sender_num) : 
    certification_processor(cp_num),
    verification_processor(vp_num),
    task_processor(tp_num),
    main_thread(1),sending_thread(sender_num)
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

void NetServer::stopAcceeption() {
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
            for(int i=0; i < MAX_USER_IN_ONCE && (n_user = SDLNet_TCP_Accept(server_socket)) != nullptr; ++i ) {
                appendUser(n_user);
            }
            change_status.store(true);
        }
    }
}

void NetServer::Reading() {
    if(change_status.load()) {
        if(set_for_users) SDLNet_FreeSocketSet(set_for_users);
        set_for_users = SDLNet_AllocSocketSet(users.size());
        for(auto& u : users) SDLNet_TCP_AddSocket(set_for_users,u.second->socket);
    }
    resetBuff();
    int active_socket = SDLNet_CheckSockets(set_for_users,0);
    for(UserIter u_itrer = users.begin(); u_itrer != users.end() && active_socket; ) {
        if(SDLNet_SocketReady(u_itrer->second->socket)) {
            active_socket--;
            if(SDLNet_TCP_Recv(u_itrer->second->socket,buffer,SERVER_BUFF_SIZE) > 0) {
                if(u_itrer->second->user_status == valid) {
                    task_processor.pushMembTask(messageProcessor,this,std::string(buffer),u_itrer->second);
                } else if(u_itrer->second->user_status == certified) {
                    verification_processor.pushMembTask(messageProcessor,this,std::string(buffer),u_itrer->second);
                } else {
                    certification_processor.pushMembTask(Certifier,this,std::string(buffer),u_itrer->second);
                }
                resetBuff();
                u_itrer++;
            } else {
                if(u_itrer->second->user_status == certified || u_itrer->second->user_status == valid) {
                    user_listener->UserQuit(u_itrer->second->id);
                }
                u_itrer = rmUser(u_itrer);
            }
        } else {
            u_itrer++;
        }
    }
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
    std::lock_guard<std::mutex> append_user_guard(user_access_mutex);
    users.emplace(cur_user_id.load(),n_user);
}

NetServer::UserIter NetServer::rmUser(UserIter deleted) {
    std::unique_lock<std::mutex> remove_user_lock(user_access_mutex);
    UserAgent* del_user = deleted->second;
    UserIter nxt = users.erase(deleted);
    remove_user_lock.unlock();
    delete del_user;
    return nxt;
}