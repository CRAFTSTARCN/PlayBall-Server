/*
    StrTool.h Created 2021/6/1
    By: Ag2S
*/

#ifndef STRTOOL_H
#define STRTOOL_J

#include <string>

class StrTool {
    public:
    static bool eqLabel(const std::string& s, const std::string& e);
    static bool validLabelContent(char c);
    static bool isSkippedChar(char c);
};

#endif