/*
    DataObject.cpp Created 2021/5/29
    By: Ag2S
*/

#include "DataObject.h"

DataObject::DataObject() = default;

DataObject::~DataObject() = default;

DataElement::DataElement() = default;

DataElement::~DataElement() = default;

DataElement::DataElement(std::string __data) : data(std::move(__data)) {}

std::string DataElement::dataType() const {
    return "ELEMENT";
}

DataSet::DataSet() = default;

DataSet::~DataSet() {
    for(auto& d : data) {
        delete d.second;
        d.second = nullptr;
    }
}

std::string DataSet::dataType() const {
    return "SET";
}

void DataSet::push(std::string key, DataObject* val) {
    data.emplace(std::move(key),val);
}