/*
    MessageParser.h Created 2021/6/8
    By: Ag2S
*/

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "AbstractMessage.h"

/*
    Message Parser
    Provide function : generate message from str
                       generate message from DataObject
    Should provide parser for each type of message
*/

class MessageParser {

public:

    static DataObject* generalStrParser(const std::string str, int& st);

    virtual AbstractMessage* parse(unsigned int sender,int v_stamp, 
                                const std::string& content) const = 0;
    virtual AbstractMessage* fromDataObject(unsigned int receiver,int v_stamp, 
                                DataObject* content) const = 0;
};

#endif