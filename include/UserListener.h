/*
    UserListener.h Created 2021/6/13
    By: Ag2S
*/

#ifndef USERLISTENER_H
#define USERLISTENER_H

#include "UserListenerInterface.h"

class Game;

class UserListener : public UserListenerInterface {
    Game* banded_game;

    public:
    UserListener(Game* __banded);
    ~UserListener();

    void UserCertified(unsigned int id);
    void UserQuit(unsigned int id);
};

#endif