/*
    Game.h Created 2021/6/11
    By: Ag2S
*/

#ifndef GAME_H
#define GAME_H

#include "NetServer.h"
#include "MessageParseLayer.h"
#include "Room.h"
#include "Player.h"
#include "ServerListener.h"
#include "UserListener.h"
#include "MessageListener.h"


#include <map>


/*
    Game layer
*/

typedef void(Game::*Func)(AbstractMessage*);

class Game {
    static std::map<std::string,Func> func_talbe;
    std::map<unsigned int, Player*> players;
    std::map<int, Room*> rooms;

    std::atomic<int> cur_room_id;

    ServerListener* server_listener;
    UserListener* user_listener;
    MessageListener* message_listener;

    ExclusiveSharedLock player_access_lock;
    ExclusiveSharedLock room_access_lock;
    
    ThreadPool resource_control_thread;

    NetServer server;
    MessageParseLayer parser;

    void appendPlayer(unsigned int id);
    void rmPlayer(unsigned int id);
    void appendRoom(unsigned int owner_id);
    void rmRoom(int room_id);
    void playerLeaveRoom(unsigned int player_id, int room_id);


    public:
    Game();
    ~Game();

    void playerJoinServer(unsigned int id);
    void playerQuitServer(unsigned int id);

    void create(AbstractMessage* msg);
    void join(AbstractMessage* msg);
    void quit(AbstractMessage* msg);
    void update(AbstractMessage* msg);
    void operate(AbstractMessage* msg);

    void processMessage(AbstractMessage* msg);
};

#endif