/*
    Game.cpp Created 2021/6/13
    By: Ag2S
*/

#include "Game.h"
#include "ACKMessage.h"
#include "GTPMessage.h"
#include "ServiceInfo.h"

Game::Game() : 
user_listener(new UserListener(this)),
message_listener(new MessageListener(this)),
parser(message_listener),
server_listener(new ServerListener(&parser)),
resource_control_thread(1),
server(1,1,2,2)
{
    cur_room_id.store(0);
    server.start(server_listener,user_listener,8000);
    server.stopVerify();
}

void Game::appendPlayer(unsigned int id) {
    player_access_lock.ExclusiveLock();
    if(players.find(id) == players.end()) {
        players.emplace(id,new Player);
    }
    player_access_lock.ExclusiveUnlock();
    server.setValid(id);
    server.pushMessage(ACKMessage::validACK(id));

}

Game::~Game() {
    server.shutwdon();
    resource_control_thread.deactivate();
    delete user_listener;
    delete message_listener;
    delete server_listener;
}

void Game::rmPlayer(unsigned int id) {
    Player* del = nullptr;
    player_access_lock.ExclusiveLock();
    auto player_it = players.find(id);
    if(player_it != players.end()) {
        del = player_it->second;
        players.erase(player_it);
    }
    player_access_lock.ExclusiveUnlock();
    if(del) {
        if(del->in_room != -1) {
            playerLeaveRoom(id,del->in_room);
        }
    }
    delete del;
}

void Game::appendRoom(unsigned int owner_id) {
    int rid = 0;
    player_access_lock.SharedLock();
    auto player_it = players.find(owner_id);
    if(player_it == players.end()) {
        player_access_lock.SharedUnlock();
        return;
    }
    std::unique_lock join_lock(player_it->second->join_mutex);
    if(player_it->second->in_room != -1) {
        rid = -1;
    } else {
        rid = cur_room_id++;
        player_it->second->in_room = rid;
    }
    join_lock.unlock();
    player_access_lock.SharedUnlock();
    if(rid != -1) {
        room_access_lock.ExclusiveLock();
        rooms.emplace(rid,new Room(owner_id));
        room_access_lock.ExclusiveUnlock();
    }
    DataObject* cont = GTPMessage::createResponse(rid);
    AbstractMessage* msg = parser.getFromDataObject(owner_id,PROTOCOL_VERSION,"GTP",cont);
    server.pushMessage(msg);
}

void Game::rmRoom(int room_id) {
    Room* del = nullptr;
    room_access_lock.ExclusiveLock();
    auto room_it = rooms.find(room_id);
    if(room_it != rooms.end()) {
        del = room_it->second;
        rooms.erase(room_it);
    }
    room_access_lock.ExclusiveUnlock();
    del->player_access_lock.SharedUnlock();
    if(del->player_number > 0) {
        auto gtp_parser = parser.getParser("GTP");
        for(auto p : del->players) {
            DataObject* cont = GTPMessage::quitResponse();
            AbstractMessage* msg = gtp_parser->second->fromDataObject(p,PROTOCOL_VERSION,cont);
            server.pushMessage(msg);
        }
    }
    del->player_access_lock.SharedUnlock();
    delete del;
}

void Game::playerLeaveRoom(unsigned int player_id, int room_id) {
    room_access_lock.SharedLock();
    auto room_it = rooms.find(room_id);
    if(room_it == rooms.end()) {
        room_access_lock.SharedUnlock();
        return;
    }
    if(room_it->second->owner == player_id) {
        player_access_lock.SharedLock();
        for(auto player : room_it->second->players) {
            auto player_it = players.find(player);
            if(player_it != players.end()) {
                std::unique_lock player_lock(player_it->second->join_mutex);
                player_it->second->in_room = -1;
            }
        }
        player_access_lock.SharedUnlock();
        resource_control_thread.pushMembTask(rmRoom,this,room_id);
    } else {
        bool erased = room_it->second->quit(player_id);
        if(erased) {
            room_it->second->player_access_lock.SharedLock();
            auto gtp_parser = parser.getParser("GTP");
             server.pushMessage(gtp_parser->second->fromDataObject(
                player_id,PROTOCOL_VERSION,GTPMessage::quitResponse()));
            for(auto player : room_it->second->players) {
                server.pushMessage(gtp_parser->second->fromDataObject(
                player,PROTOCOL_VERSION,GTPMessage::quitBroadcast(player_id)));
            }
            room_it->second->player_access_lock.SharedUnlock();
        }
    }
    
    room_access_lock.SharedUnlock();
}

void Game::playerJoinServer(unsigned int id) {
    resource_control_thread.pushMembTask(appendPlayer,this,id);
}

void Game::playerQuitServer(unsigned int id) {
    resource_control_thread.pushMembTask(rmPlayer,this,id);
}

void Game::create(AbstractMessage* msg) {
    unsigned int player_id = msg->sender_id;
    delete msg;
    resource_control_thread.pushMembTask(appendRoom,this,player_id);
}

void Game::join(AbstractMessage* msg) {
    unsigned int player_id = msg->sender_id;
    int room_id = -1;
    if(msg->content->dataType() == "SET") {
        DataObject* dobj = dynamic_cast<DataSet*>(msg->content)->find("room");
        if(dobj->dataType() == "ELEMENT") {
            room_id = std::atoi(dynamic_cast<DataElement*>(dobj)->data.c_str());
        } else {
            delete msg;
            return;
        }
    } else {
        delete msg;
        return;
    }
    delete msg;
    room_access_lock.SharedLock();
    player_access_lock.SharedLock();
    auto player_it = players.find(player_id);
    if(player_it == players.end()) {
        player_access_lock.SharedUnlock();
        room_access_lock.SharedUnlock();
        return;
    }
    player_it->second->join_mutex.lock();
    if(player_it->second->in_room == -1) {
        auto room_it = rooms.find(room_id);
        if(room_it != rooms.end() && room_it->second->join(player_id)) {
            player_it->second->in_room = room_it->first;
            server.pushMessage(parser.getFromDataObject(
                player_id,PROTOCOL_VERSION,"GTP",GTPMessage::joinResponse(room_it->first)
            ));
            room_it->second->player_access_lock.SharedLock();
            for(auto player : room_it->second->players) {
                if(player == player_id) continue;
                auto gtp_parser = parser.getParser("GTP");
                server.pushMessage(gtp_parser->second->fromDataObject(
                    player,PROTOCOL_VERSION,GTPMessage::joinBroadcast(player_id)
                ));
            }
            room_it->second->player_access_lock.SharedUnlock();
        } else {
            server.pushMessage(parser.getFromDataObject(
                player_id,PROTOCOL_VERSION,"GTP",GTPMessage::joinResponse(-1)
            ));
        }
    } else {
        server.pushMessage(parser.getFromDataObject(
            player_id,PROTOCOL_VERSION,"GTP",GTPMessage::joinResponse(-1)
        ));
    }
    player_it->second->join_mutex.unlock();
    player_access_lock.SharedUnlock();
    room_access_lock.SharedUnlock();
}

void Game::quit(AbstractMessage* msg) {
    unsigned int player_id = msg->sender_id;
    int player_room = -1;
    delete msg;
    player_access_lock.SharedLock();
    auto player_it = players.find(player_id);
    if(player_it == players.end()) {
        player_access_lock.SharedUnlock();
        return;
    }
    player_it->second->join_mutex.lock();
    player_room = player_it->second->in_room;
    player_it->second->in_room = -1;
    player_it->second->join_mutex.unlock();
    player_access_lock.SharedUnlock();
    if(player_room != -1) {
        playerLeaveRoom(player_id,player_room);
    } else {
        server.pushMessage(parser.getFromDataObject(player_id,PROTOCOL_VERSION,"GTP",GTPMessage::quitResponse()));
    }
}

void Game::operate(AbstractMessage* msg) {
    int id = msg->sender_id;
    player_access_lock.SharedLock();
    auto player_it = players.find(id);
    if(player_it == players.end()) {
        player_access_lock.SharedUnlock();
        delete msg;
        return;
    }
    player_it->second->join_mutex.lock();
    int in_room = player_it->second->in_room;
    player_it->second->join_mutex.unlock();
    player_access_lock.SharedUnlock();
    if(in_room == -1) {
        delete msg;
        return;
    }
    room_access_lock.SharedUnlock();
    auto room_it = rooms.find(in_room);
    if(room_it != rooms.end()) {
        room_it->second->player_access_lock.SharedLock();
        auto owner = room_it->second->owner;
        room_it->second->player_access_lock.SharedUnlock();
        DataObject* cont = msg->content;
        msg->content = nullptr;
        server.pushMessage(parser.getFromDataObject(owner,PROTOCOL_VERSION,"GTP",cont));
    }
    room_access_lock.SharedUnlock();
    delete msg;
}

void Game::update(AbstractMessage* msg) {
    int id = msg->sender_id;
    player_access_lock.SharedLock();
    auto player_it = players.find(id);
    if(player_it == players.end()) {
        player_access_lock.SharedUnlock();
        delete msg;
        return;
    }
    player_it->second->join_mutex.lock();
    int in_room = player_it->second->in_room;
    player_it->second->join_mutex.unlock();
    player_access_lock.SharedUnlock();
    if(in_room == -1) {
        delete msg;
        return;
    }
    room_access_lock.SharedUnlock();
    auto room_it = rooms.find(in_room);
    if(room_it != rooms.end() && room_it->second->owner == id) {
        room_it->second->player_access_lock.SharedLock();
        for(auto player : room_it->second->players) {
            if(player == id) continue;
            server.pushMessage(parser.getFromDataObject(player,PROTOCOL_VERSION,"GTP",msg->content->SafeCopy()));
        }
        room_it->second->player_access_lock.SharedUnlock();
    }
    room_access_lock.SharedUnlock();
    delete msg;
}

void Game::processMessage(AbstractMessage* msg) {
    bool run  = false;
    if(msg->content->dataType() == "SET") {
        DataObject* dobj = dynamic_cast<DataSet*>(msg->content)->find("option");
        if(dobj->dataType() == "ELEMENT") {
            std::string opt = dynamic_cast<DataElement*>(dobj)->data;
            auto func = Game::func_talbe.find(opt);
            if(func != func_talbe.end()) {
                run = true;
                Func f = func->second;
                (this->*f)(msg);
            }
        }
    }
    if(!run) {
        delete msg;
    }
}

std::map<std::string,Func> Game::func_talbe = {
    {"CREATE",&Game::create},
    {"JOIN",&Game::join},
    {"QUIT",&Game::quit},
    {"UPDATE",&Game::update},
    {"OPERATE",&Game::operate}
};