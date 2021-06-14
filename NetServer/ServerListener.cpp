/*
    ServerListener.h Created 2021/6/13
    By: Ag2S
    Impliment of ServerListenerInterface
*/

#include "ServerListener.h"

ServerListener::ServerListener(MessageParseLayer* __banded) : banded_parser_layer(__banded) {}

ServerListener::~ServerListener() = default;

void ServerListener::onEmitMessage(unsigned int sender,int v_stamp, 
    const std::string& p_type, const std::string& content)
{
    banded_parser_layer->processEmit(sender, v_stamp, p_type, content);
}