/*
    GTPParser.h Created 2021/6/10
    By: Ag2S
*/

#ifndef GTPPARSER_H
#define GTPPARSER_H

#include "MessageParser.h"

class GTPParser : public MessageParser {
    
    public:

    static DataObject* gtpParser(const std::string& content);

    AbstractMessage* parse(unsigned int sender,int v_stamp, 
                        const std::string& content) const;
    AbstractMessage* fromDataObject(unsigned int receiver,int v_stamp,
                        DataObject* content) const;

};

#endif