/*
    Room.h Created 2021/6/11
    By: Ag2S
*/

#ifndef ROOM_H
#define ROOM_H

/*
    Game component room
*/

#include "ExclusiveSharedLock.h"

#include <set>
#include <vector>

class Room {
    unsigned int owner;
    std::set<unsigned int> players;
    int player_number;
    ExclusiveSharedLock player_access_lock;

    public:
    Room(unsigned int __owner);
    ~Room();

    bool join(unsigned int player_id);
    bool quit(unsigned int player_id);
    friend class Game;
};
#endif