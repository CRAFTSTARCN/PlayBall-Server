/*
    StrTool.cpp Created 2021/6/1
    By: Ag2S
*/

#include "StrTool.h"

bool StrTool::eqLabel(const std::string& s, const std::string& e) {
    if(s.length() + 1 != e.length()) return false;
    if(e[0] != '/') return false;
    for(int i=0; i<s.length(); ++i) {
        if(s[i] != e[i+1]) return false;
    }
    return true;
}

bool StrTool::validLabelContent(char c) {
    return ('A' <= c && c <= 'Z') 
    || ('a' <= c && c <= 'z') 
    || ('1' <= c && c <= '9')
    || c == '_';
}