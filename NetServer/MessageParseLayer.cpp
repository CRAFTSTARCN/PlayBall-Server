/*
    MessageParseLayer.cpp Created 2021/6/10
    By: Ag2S
*/

#include "MessageParseLayer.h"

MessageParseLayer::MessageParseLayer(MessageListenerInterFace* __listener) : listener(__listener) {

}

MessageParseLayer::~MessageParseLayer() {
    for(auto& parser : parsers) {
        delete parser.second;
    }
}

void MessageParseLayer::processEmit(unsigned int sender,int v_stamp, 
    const std::string& p_type, const std::string& content) 
{
    auto parser_it = parsers.find(p_type);
    if(parser_it == parsers.end()) return;
    AbstractMessage* parsered_msg = parser_it->second->parse(sender,v_stamp,content);
    if(!parsered_msg) return;
    listener->onEmitMessage(parsered_msg);
}

AbstractMessage*  MessageParseLayer::getFromDataObject(unsigned int sender,int v_stamp, 
    const std::string& p_type, DataObject* content)
{
    auto parser_it = parsers.find(p_type);
    if(parser_it == parsers.end()) return;
    AbstractMessage* parsered_msg = parser_it->second->fromDataObject(sender,v_stamp,content);
    return parsered_msg;
}