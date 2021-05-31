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

#include "ThreadPool.h"
#include "UserAgent.h"

#include <SDL2/SDL_net.h>

#include "InterceptorInterface.h"
#include "AbstractMessage.h"

#include <vector>

class NetServer {
    TCPsocket server_socket;
    IPaddress server_ip_port;
    std::vector<UserAgent*> users;
    
    ThreadPool certification_processor;
    ThreadPool verification_processor;
    ThreadPool task_processor;

    ThreadPool reading_thread;
    ThreadPool listening_thread;
    ThreadPool sending_thread;

    InterceptorInterface* filter;

    std::atomic<unsigned int> cur_user_id;

    std::atomic<bool> is_open;
    std::atomic<bool> accept_new;

    std::mutex user_access_mutex;

    void appendUser(TCPsocket user_socket);

    std::map<std::string,std::string> headAnalyzer(const std::string& message);
    void messageProcessor(std::string&& msg, UserAgent* sender);
    void Certifier(std::string&& msg, UserAgent sender);

    void sendMsg(AbstractMessage* msg);

    void Listening();
    void Reading();


    public:

    NetServer(InterceptorInterface* __filter, int cp_num = 1, int vp_num = 2, int tp_num = 2, int sender_num = 1);
    ~NetServer();

    /*
        start server fucntion
        server will accept new socket and reading messages after call
        Should be called when server down and after SDL_NET initialized
    */
    int start(Uint16 port);

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
};

#endif