/*
    GTPMessage.h Created 2021/6/8
    By: Ag2S
*/

#ifndef GTPMESSAGE_H
#define GTPMESSAGE_H

/*
    Game Transport Protocol Message
    Special : No special char in message body
*/

#include "AbstractMessage.h"
class GTPMessage : public AbstractMessage {

    public:
    GTPMessage();
    GTPMessage(DataObject* __content);
    ~GTPMessage();

    std::string protocolType() const;
};

#endif