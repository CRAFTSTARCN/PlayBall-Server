/*
    UserAgent.h Created 2021/5/28
    By: Ag2s
*/

#ifndef USERAGENT_H
#define USERAGENT_H

#include <SDL2/SDL_net.h>

#include <vector>
#include <mutex>

enum UserStatus {
    uncertified, certified, valid
};

class UserAgent {
    public:

    unsigned int id;
    TCPsocket socket;
    std::string* buffer;
    
    UserStatus user_status;

    UserAgent(unsigned int __id, TCPsocket __socket);
    ~UserAgent();

};

#endif