/*
    NetServer.cpp Created 2021/5/27
    By: Ag2S
*/

#include "NetServer.h"

NetServer::NetServer(InterceptorInterface* __filter, int cp_num, int vp_num, int tp_num, int sender_num) : 
    filter(__filter),
    certification_processor(cp_num),
    verification_processor(vp_num),
    task_processor(tp_num),
    listening_thread(1),reading_thread(1),sending_thread(sender_num)
{
    cur_user_id.store(0);
    is_open.store(false);
    users.push_back(nullptr);
}

NetServer::~NetServer() {
    shutwdon();
}

int NetServer::start(Uint16 port) {
    if(is_open.load()) return SERVER_HAS_BEEN_OPEN;

    int ip_status = SDLNet_ResolveHost(&server_ip_port,nullptr,port);
    if(ip_status!=0) return HOST_RESOLVE_FAIL;

    server_socket = SDLNet_TCP_Open(&server_ip_port);
    if(!server_socket) return OPEN_TCPSOCKET_FAIL;

    is_open.store(1);
    accept_new.store(1);

    listening_thread.pushMembTask(Listening,this);
    reading_thread.pushMembTask(Reading,this);

    return OK;
}

void NetServer::stopAcceeption() {
    if(!accept_new.load());

    accept_new.store(false);
    listening_thread.deactivate();

    return;
}

void NetServer::shutwdon() {
    stopAcceeption();
    
    is_open.store(false);
    reading_thread.deactivate();

    std::unique_lock<std::mutex> last_lock;
    /* send to all */

    certification_processor.forceShutdown();
    verification_processor.deactivate();
    task_processor.deactivate();
    sending_thread.deactivate();
    return;
}

void NetServer::Listening() {
    TCPsocket n_client_socket;
    while(accept_new.load()) {
        n_client_socket = SDLNet_TCP_Accept(server_socket);
        if(n_client_socket) certification_processor.pushMembTask(appendUser,this,n_client_socket);
    }
}

void NetServer::Reading() {
    while(is_open.load()) {
        
    }
}

void NetServer::appendUser(TCPsocket user_socket) {
    std::lock_guard<std::mutex> user_access_guard(user_access_mutex);
    cur_user_id++;
    UserAgent *n_user = new UserAgent;
    n_user->id = cur_user_id.load();
    n_user->socket = user_socket;
    n_user->user_status = uncertified;
    users.push_back(n_user);
}