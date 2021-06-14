/*
    ServerListener.h Created 2021/6/13
    By: Ag2S
    Impliment of ServerListenerInterface
*/

#ifndef SERVERLISTENER_H
#define SERVERLISTENER_H

#include "NetServer.h"
#include "MessageParseLayer.h"

class ServerListener : public ServerListenerInterface {
    MessageParseLayer* banded_parser_layer;

    public:
    ServerListener(MessageParseLayer* __banded);
    ~ServerListener();

    void onEmitMessage(unsigned int sender,int v_stamp, 
                               const std::string& p_type, const std::string& content);
};




#endif