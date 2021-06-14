/*
    ACKMessage.h Created 2021/6/13
    By: Ag2S
*/

#ifndef ACKMESSAGE_H
#define ACKMESSAGE_H

#include "AbstractMessage.h"

class ACKMessage : public AbstractMessage {
    public:
    ACKMessage();
    ACKMessage(DataObject* __content);
    ~ACKMessage();

    std::string protocolType() const;

    static std::string ACK(unsigned int id);
    static ACKMessage* validACK(unsigned int id);
};

#endif