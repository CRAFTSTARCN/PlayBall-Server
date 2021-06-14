/*
    Game.h Created 2021/6/11
    By: Ag2S
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <mutex>

class Player {
    public:
    int in_room;
    std::mutex join_mutex;

    Player();
    ~Player();
};

#endif