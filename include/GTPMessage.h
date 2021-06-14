/*
    GTPMessage.h Created 2021/6/8
    By: Ag2S
*/

#ifndef GTPMESSAGE_H
#define GTPMESSAGE_H

/*
    Game Transport Protocol Message
    Special : No special char in message body
              No label nesting in message
*/

#include "AbstractMessage.h"
class GTPMessage : public AbstractMessage {

    public:
    GTPMessage();
    GTPMessage(DataObject* __content);
    ~GTPMessage();

    std::string protocolType() const;

    static DataObject* createResponse(int code);
    static DataObject* joinResponse(int code);
    static DataObject* joinBroadcast(unsigned int id);
    static DataObject* quitResponse();
    static DataObject* quitBroadcast(unsigned int id);
};

#endif