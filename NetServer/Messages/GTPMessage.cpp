/*
    GTPMessage.cpp Created 2021/6/8
    By: Ag2S
*/

#include "GTPMessage.h"

GTPMessage::GTPMessage() = default;

GTPMessage::GTPMessage(DataObject* __content) : AbstractMessage(__content) {}

std::string GTPMessage::protocolType() const {
    return "GTP";
}