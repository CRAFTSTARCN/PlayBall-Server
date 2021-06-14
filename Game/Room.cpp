/*
    Room.cpp Created 2021/6/12
    By: Ag2S
*/


#include "Room.h"

Room::Room(unsigned int  __owner) : owner(__owner) {
    players.insert(__owner);
    player_number = 1;
}

Room::~Room() = default;

bool Room::join(unsigned int player_id) {
    player_access_lock.ExclusiveLock();
    if(player_number >= 5 || players.find(player_id) != players.end()) {
        player_access_lock.ExclusiveUnlock();
        return false;
    }
    players.insert(player_id);
    player_access_lock.ExclusiveUnlock();
    return true;
}

bool Room::quit(unsigned int player_id) {
    player_access_lock.ExclusiveLock();
    auto player_it = players.find(player_id);
    bool ret = player_it != players.end();
    if(ret) {
        player_number--;
        players.erase(player_it);
    }
    player_access_lock.ExclusiveUnlock();
    return ret;
}