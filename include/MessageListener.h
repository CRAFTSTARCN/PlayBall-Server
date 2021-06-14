/*
    MessageListener.h Created 2021/6/13
    By: Ag2S
*/

#ifndef MESSAGELISTENER_J
#define MESSAGELISTENER_J

#include "MessageListenerInterface.h"

class Game;

class MessageListener : public MessageListenerInterface {
    Game* banded_game;

    public:
    MessageListener(Game* __banded);
    ~MessageListener();

    void onEmitMessage(AbstractMessage* msg);
};

#endif