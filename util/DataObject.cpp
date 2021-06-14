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

std::string DataElement::toString() const {
    return data;
}

DataObject* DataElement::SafeCopy() const {
    return new DataElement(data);
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

std::string DataSet::toString() const {
    std::string res;
    for(auto &d : data) {
        res.append("<" + d.first + ">");
        res.append(d.second->toString());
        res.append("</" + d.first + ">");
    }
    return res;
}

DataObject* DataSet::SafeCopy() const {
    DataSet* n_set = new DataSet;
    for(auto &dobj : data) {
        n_set->push(dobj.first,dobj.second->SafeCopy());
    }
    return n_set;
}

void DataSet::push(std::string key, DataObject* val) {
    auto data_it = data.find(key);
    if(data_it == data.end()) data.emplace(key,val);
    else{
        delete data_it->second;
        data_it->second = val;
    }
}

DataObject* DataSet::find(const std::string& key) {
    auto data_it = data.find(key);
    if(data_it == data.end()) return nullptr;
    return data_it->second;
}