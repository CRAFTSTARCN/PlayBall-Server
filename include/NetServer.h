/*
    NetServer.h Created 2021/5/27
    Include file for NetServer.cpp 
    By: Ag2s
*/


#ifndef NETSERVER_H
#define NETSERVER_H

#include "ThreadPool.h"
#include "UserAgent.h"

#include <SDL2/SDL_net.h>

#include <map>
#include <set>

class NetServer {
    TCPsocket server_socket;
    IPaddress server_ip_port;
    std::map<unsigned int, UserAgent> users;
    
    ThreadPool certification_processor;
    ThreadPool verification_processor;
    ThreadPool task_processor;

    ThreadPool reading_thread;
};

#endif