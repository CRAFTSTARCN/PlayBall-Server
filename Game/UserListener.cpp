/*
    UserListener.cpp Created 2021/6/13
    By: Ag2S
*/

#include "UserListener.h"
#include "Game.h"

UserListener::UserListener(Game* __banded) : banded_game(__banded) {}

UserListener::~UserListener() = default;

void UserListener::UserCertified(unsigned int id) {
    banded_game->playerJoinServer(id);
}

void UserListener::UserQuit(unsigned int id) {
    banded_game->playerQuitServer(id);
}