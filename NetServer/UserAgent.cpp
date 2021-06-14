/*
    UserAgent.cpp Created 2021/6/7
    By: Ag2s
*/

#include "UserAgent.h"

UserAgent::UserAgent(unsigned int __id, TCPsocket __socket) :
    id(__id), socket(__socket),buffer(nullptr),user_status(uncertified)
{}

UserAgent::~UserAgent() {
    delete buffer;
    SDLNet_TCP_Close(socket);
}