/*
    DataObject.h Created 2021/5/29
    By: Ag2S
*/

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <string>
#include <map>

class DataObject {
    
    public:
    DataObject();
    virtual ~DataObject();

    virtual std::string dataType() const = 0;

};

class DataElement final : public DataObject  {

    public:
    std::string data;

    DataElement();
    explicit DataElement(std::string __data);
    ~DataElement();

    std::string dataType() const;
};


class DataSet final : public DataObject {

    using m_key =  std::string;

    public:
    std::map<m_key,DataObject*> data;
    void push(m_key key, DataObject* val);

    DataSet();
    ~DataSet();

    std::string dataType() const;

};

#endif