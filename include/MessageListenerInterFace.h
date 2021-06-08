/*
    MessageListenerInterface.h Created 2021/6/8
    By: Ag2S
*/

#ifndef MESSAGELISTENERINTERFACE_H
#define MESSAGELISTENERINTERFACE_H

#include "CallbackListener.h"
#include "AbstractMessage.h"

class MessageListenerInterFace : public CallbackListener {

    public:
    virtual void onEmitMessage(AbstractMessage* msg);
};

#endif