/*
    MessageListenerInterface.h Created 2021/6/8
    By: Ag2S
*/

#ifndef MESSAGELISTENERINTERFACE_H
#define MESSAGELISTENERINTERFACE_H

#include "CallbackListener.h"
#include "AbstractMessage.h"

class MessageListenerInterface : public CallbackListener {

    public:
    MessageListenerInterface();
    ~MessageListenerInterface();

    virtual void onEmitMessage(AbstractMessage* msg) = 0;
};

#endif