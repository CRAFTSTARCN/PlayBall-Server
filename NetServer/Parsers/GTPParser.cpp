/*
    GTPParser.cpp Created 2021/6/14
    By: Ag2S
*/

#include "GTPParser.h"
#include "GTPMessage.h"
#include "StrTool.h"

/*
    Function implimented by @ Xi Chen.
*/
DataObject* GTPParser::gtpParser(const std::string& content) {
    int i = 0;
	bool flg = 1;
	DataSet* set = new DataSet;
	
	while(i<content.length()){
		if(content[i]=='<'){
			i++;
			std::string sign,tmp,end;
			while(content[i]!='>'&&i<content.length()){
				flg = StrTool::validLabelContent(content[i]);
				sign += content[i];
				i++;
			}
			if(i>=content.length()){
				flg = 0;
				break;
			}
			i++;
			while(content[i]!='<'&&i<content.length()){
				if(StrTool::isSkippedChar(content[i])){
					
					i++;
					continue;
				}
				tmp += content[i];
				i++;
			}
			i++;
			if(content[i]!='/') flg = 0;
			i++;
			if(i>=content.length()){
						flg = 0;
						break;
				}
			while(content[i]!='>'&&i<content.length()){
				end += content[i];
				i++;
			}
			if(end!=sign){
				flg = 0;
			}
			set->push(sign,new DataElement(tmp));
			if(flg==0) break;
			while(content[i]!='<'&&i<content.length()){
				i++;
			} 
		}
		else{
			i++;
		}
	}
	
	if(!flg){
		delete set;
		return nullptr;
	}
	else return set;
}

AbstractMessage* GTPParser::parse(unsigned int sender,int v_stamp, 
    const std::string& content) const 
{
    DataObject* cont = gtpParser(content);
    if(cont == nullptr) return nullptr;
    AbstractMessage* msg = new GTPMessage(cont);
    msg->sender_id = sender;
    msg->version_stamp = v_stamp;
    return msg;
}

AbstractMessage* GTPParser::fromDataObject(unsigned int receiver,int v_stamp,
    DataObject* content) const 
{
    AbstractMessage* msg = new GTPMessage(content);
    msg->version_stamp = v_stamp;
    msg->to_id = receiver;
    return msg;
}