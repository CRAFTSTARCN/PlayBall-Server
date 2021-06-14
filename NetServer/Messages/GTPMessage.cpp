/*
    GTPMessage.cpp Created 2021/6/8
    By: Ag2S
*/

#include "GTPMessage.h"

GTPMessage::GTPMessage() = default;

GTPMessage::GTPMessage(DataObject* __content) : AbstractMessage(__content) {}

GTPMessage::~GTPMessage() = default;

std::string GTPMessage::protocolType() const {
    return "GTP";
}

DataObject* GTPMessage::createResponse(int code) {
    DataSet* cont = new DataSet;
    cont->push("option", new DataElement("CREATE"));
    cont->push("success", new DataElement(std::to_string(code)));
    return cont;
}

DataObject* GTPMessage::joinResponse(int code) {
    DataSet* cont = new DataSet;
    cont->push("option", new DataElement("JOIN"));
    cont->push("room", new DataElement(std::to_string(code)));
    return cont;
}

DataObject* GTPMessage::joinBroadcast(unsigned int id) {
    DataSet* cont = new DataSet;
    cont->push("option", new DataElement("UJOIN"));
    cont->push("playerid", new DataElement(std::to_string(id)));
    return cont;
}

DataObject* GTPMessage::quitBroadcast(unsigned int id) {
    DataSet* cont = new DataSet;
    cont->push("option", new DataElement("UQUIT"));
    cont->push("playerid", new DataElement(std::to_string(id)));
    return cont;
}

DataObject* GTPMessage::quitResponse() {
    DataSet* cont = new DataSet;
    cont->push("option", new DataElement("QUIT"));
    return cont;
}