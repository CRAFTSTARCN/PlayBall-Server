/*
    AbstractMessage.h  Created 2021/5/25
    By: Ag2S
*/

#ifndef ABSTRACTMESSAGE_H
#define ABSTRACTMESSAGE_H

#include "DataObject.h"

class AbstractMessage {

    public:
    unsigned int sender_id;
    unsigned int to_id;
    int version_stamp;

    DataSet* body;

    AbstractMessage();
    virtual ~AbstractMessage();

    virtual std::string protocolType() const = 0;
    virtual std::string toText() const;
};

#endif