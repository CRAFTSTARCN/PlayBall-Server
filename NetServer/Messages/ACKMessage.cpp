/*
    ACKMessage.cpp Created 2021/6/13
    By: Ag2S
*/

#include "ACKMessage.h"
#include "ServiceInfo.h"

ACKMessage::ACKMessage() = default;

ACKMessage::ACKMessage(DataObject* __content) : AbstractMessage(__content) {}

ACKMessage::~ACKMessage() = default;

std::string ACKMessage::protocolType() const {
    return "ACK";
}

std::string ACKMessage::ACK(unsigned int id) {
    return "<msg><head><id>" + 
    std::to_string(id) + "</id><version>" + std::to_string(PROTOCOL_VERSION) + "</version><protocolType>ACK</protocolType></head></msg>";
}

ACKMessage* ACKMessage::validACK(unsigned int id) {
    DataSet* cont = new DataSet;
    cont->push("joinserver",new DataElement("valid"));
    ACKMessage* ack = new ACKMessage(cont);
    ack->to_id = id;
    return ack;
}