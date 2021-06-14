/*
    MessageListener.cpp Created 2021/6/13
    By: Ag2S
*/

#include "MessageListener.h"
#include "Game.h"

MessageListener::MessageListener(Game* __banded) : banded_game(__banded) {}

MessageListener::~MessageListener() = default;

void MessageListener::onEmitMessage(AbstractMessage* msg) {
    banded_game->processMessage(msg);
}