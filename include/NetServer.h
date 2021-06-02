/*
    NetServer.h Created 2021/5/27
    Include file for NetServer.cpp 
    By: Ag2s
*/


#ifndef NETSERVER_H
#define NETSERVER_H

#define HOST_RESOLVE_FAIL 1
#define OPEN_TCPSOCKET_FAIL 2
#define SERVER_HAS_BEEN_OPEN -1
#define OK 0

#define MAX_USER_IN_ONCE 100
#define SERVER_BUFF_SIZE 512

#include "ThreadPool.h"
#include "UserAgent.h"

#include <SDL2/SDL_net.h>

#include "InterceptorInterface.h"
#include "ServerListenerInterface.h"
#include "UserListenerInterface.h"
#include "AbstractMessage.h"

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

    InterceptorInterface* filter;
    ServerListenerInterface* listener;
    UserListenerInterface* user_listener;

    std::atomic<unsigned int> cur_user_id;

    std::atomic<bool> is_open;
    std::atomic<bool> accept_new;
    std::atomic<bool> change_status;

    std::mutex user_access_mutex;

    inline void resetBuff();

    void appendUser(TCPsocket user_socket);
    UserIter rmUser(UserIter deleted);

    std::map<std::string,std::string> headAnalyzer(const std::string& message);
    void messageProcessor(std::string&& msg, UserAgent* sender);
    void Certifier(std::string&& msg, UserAgent sender);

    void sendMsg(AbstractMessage* msg);

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
             InterceptorInterface* __filter, 
             Uint16 port = 8000);

    /* stop new socket */
    void stopAcceeption();

    /* shutdown server */
    void shutwdon();

    /* set a user as certified user */
    void setCertified(int uid);

    /* set a user as a valid user */
    void setValid(int uid);

    /* push a message to sender thread */
    void pushMessage(AbstractMessage* msg);

    /*
        set max new user one each loop 
        default 128
    */
    void setMaxNewUserOnce(int num);
};

#endif