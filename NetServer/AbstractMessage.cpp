/*
    AbstractMessage.cpp  Created 2021/6/26
    By: Ag2S
*/

#include "AbstractMessage.h"
#include "ServiceInfo.h"

AbstractMessage::AbstractMessage() {
    content = nullptr;
    sender_id = 0; to_id = 0;
    version_stamp = PROTOCOL_VERSION;
}

AbstractMessage::AbstractMessage(DataObject* __content) : content(__content) {
    sender_id = 0; to_id = 0;
    version_stamp = PROTOCOL_VERSION;
}

AbstractMessage::~AbstractMessage() {
    delete content;
}

std::string AbstractMessage::toText() const {
    std::string text;
    text.append("<msg><head><id>");
    if(sender_id == 0)  text.append(std::to_string(to_id));
    else text.append(std::to_string(sender_id));
    text.append("</id>")
        .append("<version>").append(std::to_string(version_stamp)).append("</version>")
        .append("<protocolType>").append(this->protocolType()).append("/<protocolType>")
        .append("</head><content>")
        .append(content->toString()).append("</content></msg>");
    return text;
}