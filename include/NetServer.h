/*
    NetServer.h Created 2021/5/27
    Include file for NetServer.cpp 
    By: Ag2s
*/


#ifndef NETSERVER_H
#define NETSERVER_H

#include "ThreadPool.h"
#include "ExclusiveSharedLock.h"
#include "UserAgent.h"
#include "ServerListenerInterface.h"
#include "UserListenerInterface.h"
#include "AbstractMessage.h"

#include <SDL2/SDL_net.h>

#include <map>

class NetServer {
    public:
    using UserIter = std::map<unsigned int,UserAgent*>::iterator;

    private:
    TCPsocket server_socket;
    IPaddress server_ip_port;
    
    SDLNet_SocketSet set_for_server;
    SDLNet_SocketSet set_for_users;

    char* buffer;

    std::map<unsigned int,UserAgent*> users;
    
    ThreadPool certification_processor;
    ThreadPool verification_processor;
    ThreadPool task_processor;

    ThreadPool main_thread;
    ThreadPool sending_thread;
    ThreadPool resource_reclamation_thread;

    ServerListenerInterface* listener;
    UserListenerInterface* user_listener;

    unsigned int cur_user_id;

    std::atomic<bool> is_open;
    std::atomic<bool> accept_new;
    std::atomic<bool> is_verifying;
    std::atomic<bool> is_certifying;

    bool change_status;

    ExclusiveSharedLock user_access_lock;

    inline void resetBuff();

    void appendUser(TCPsocket user_socket);
    void rmUsers(const std::vector<unsigned int>& removed_users);

    std::vector<std::string> cut(const std::string& msg, int& vis);
    std::map<std::string,std::string> headAnalyzer(const std::string& message);
    void messageProcessor(const std::string& msg, UserAgent* user);
    void Certifier(const std::string& msg, unsigned int id);

    void sendMsg(AbstractMessage* msg);
    void sendStr(const std::string& str, int id);

    void Listening();
    void Reading();
    void mainLoop();

    public:

    NetServer(int cp_num = 1, int vp_num = 2, int tp_num = 2, int sender_num = 1);
    ~NetServer();

    /*
        start server fucntion
        server will accept new socket and reading messages after call
        Should be called when server down and after SDL_NET initialized
    */
    int start(ServerListenerInterface* __listener, UserListenerInterface* __u_listener ,
             Uint16 port = 8000);

    /* stop new socket */
    void stopAccept();

    /* stop certify new connected user */
    void stopCertify();

    /* stop verify certified users */
    void stopVerify();

    /* shutdown server */
    void shutwdon();

    /* set a user as a valid user */
    void setValid(unsigned int uid);

    /* push a message to sender thread */
    void pushMessage(AbstractMessage* msg);

};

#endif