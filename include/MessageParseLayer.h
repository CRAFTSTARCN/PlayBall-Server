/*
    MessageParseLayer.h Created 2021/6/8
    By: Ag2S
*/

#ifndef MESSAGEPARSELAYER_H
#define MESSAGEPARSELAYER_H

#include "MessageParser.h"
#include "MessageListenerInterFace.h"

#include <map> 


/*
    Message paser layer
    Functions: Get text message emitation from lower layer, process and emit to upper layer
             : Provide message forming interface from upper layer
*/

class MessageParseLayer {
    std::map<std::string, MessageParser*> parsers;    
    MessageListenerInterFace* listener;

    public:
    
    MessageParseLayer(MessageListenerInterFace* __listener);
    ~MessageParseLayer();

    /* Process str, emit message to upper layer */
    void processEmit(unsigned int sender,int v_stamp, 
                     const std::string& p_type, const std::string& content);

    /* Form a message for upper layer */
    AbstractMessage* getFromDataObject(unsigned int sender,int v_stamp, 
                     const std::string& p_type, DataObject* content);
};

#endif