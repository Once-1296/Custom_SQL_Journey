#ifndef COMMAND_VALIDATION_HELPERS_HPP
#define COMMAND_VALIDATION_HELPERS_HPP
#include "../types/Token.hpp"
#include "../types/schema.hpp"
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <dirent.h>
bool isAlpha(char c) { return (c <= 'z' && c >= 'a') || (c >= 'A' && c <= 'Z'); }
bool isNum(char c) { return (c >= '0' && c <= '9'); }
bool isUnderScore(char c) { return (c == '_'); }
bool isAlphaNumUS(char c) { return isAlpha(c) || isNum(c) || isUnderScore(c); }

std::pair<bool, std::variant<std::string, int>> validTokenAt(int32_t i, std::vector<Token> &tokens,
                                                             std::string &Message, std::vector<tokenType> matchTypes, int32_t matchInd = -1, bool chkValue = false, std::variant<std::string, int> matchValue = "")
{
    if (i < 0 || i >= tokens.size())
    {
        Message = "Invalid index for some token.";
        return {false, ""};
    }
    Token &tok = tokens[i];
    bool matchedType = false;
    for (auto &matchType : matchTypes)
    {
        if (tok.type == matchType)
        {
            matchedType = true;
            break;
        }
    }
    if (!matchedType)
    {
        std::string validTypes = "";
        for (auto &matchType : matchTypes)
        {
            validTypes += printType(matchType) + " : ";
        }
        Message = "Expected token of type {" + validTypes + "} , instead got token of type " + printType(tok.type);
        return {false, ""};
    }
    if (matchInd != -1)
    {
        if (matchInd != tok.value.index())
        {
            Message = "Wrong data of token.";
            return {false, ""};
        }
        if (chkValue)
        {
            if (matchInd == 0)
            {
                if (std::get<0>(matchValue) != std::get<0>(tok.value))
                {
                    Message = "Expected " + std::get<0>(matchValue) + " ,Got " + std::get<0>(tok.value);
                    return {false, ""};
                }
            }
            else if (matchInd == 1)
            {
                if (std::get<1>(matchValue) != std::get<1>(tok.value))
                {
                    Message = "Expected " + std::to_string(std::get<1>(matchValue)) + " ,Got " + std::to_string(std::get<1>(tok.value));
                    return {false, ""};
                }
            }
            else
            {
                Message = "Runtime error.";
                return {false, ""};
            }
        }
    }
    return {true, tok.value};
}

std::pair<bool, bool> fileExists(DIR *&dir, struct dirent *&entry, std::string &file_path, std::string db_name, std::string &Message){
    // Open the current directory
    const char *path = file_path.data();
    dir = opendir(path);
    bool exists = false;
    if (dir == NULL)
    {
        Message = "Unable to open directory.";
        return {false, false};
    }
    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".db" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".db" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            if (entry->d_name == db_name)
            {
                exists = true;
                break;
            }
        }
    }
    // Close the directory
    closedir(dir);
    return {true, exists};
}

bool isValidTableName(std::string &tableName, std::string &Message){
    while(!tableName.empty() &&tableName.back() == ' ')tableName.pop_back();
    while(!tableName.empty() && *tableName.begin() == ' ')tableName.erase(tableName.begin());
    if(tableName.size() == 0){
        Message = "Table Name is empty";
        return false;
    }
    if(tableName.size() >= 64){
        Message = "Table Name is too long";
        return false;
    }
    if(!isAlpha(tableName[0]) && !isUnderScore(tableName[0])){
        Message = "Table Name must begin with Alphabet or Underscore.";
        return false;
    }
    for(auto&c:tableName){
        if(!isAlphaNumUS(c) && c!=' '){
            Message = "Invalid character " + c;
            return false;
        }
    }
    return true;
}

bool isValidColName(std::string &colName, std::string &Message){
    while(!colName.empty() &&colName.back() == ' ')colName.pop_back();
    while(!colName.empty() && *colName.begin() == ' ')colName.erase(colName.begin());
    if(colName.size() == 0){
        Message = "Column Name is empty";
        return false;
    }
    if(colName.size() >= 32){
        Message = "Column Name is too long";
        return false;
    }
    if(!isAlpha(colName[0]) && !isUnderScore(colName[0])){
        Message = "Column Name must begin with Alphabet or Underscore.";
        return false;
    }
    for(auto&c:colName){
        if(!isAlphaNumUS(c) && c!=' '){
            Message = "Invalid character " + c;
            return false;
        }
    }
    return true;
}

bool checkCol(int32_t i, std::vector<Token> &tokens, std::string &Message, std::map<std::string, std::tuple<TypeId, uint32_t, uint32_t>> &curCols, int32_t pos_i)
{
    std::string colName = std::get<0>(tokens[i].value);
    bool checkColName = isValidColName(colName, Message);
    if(!checkColName){
        return false;
    }
    if(curCols.find(colName) != curCols.end()){
        Message =  colName + " Column Name is repeated.";
        return false;
    }
    auto checkType = validTokenAt(i+1, tokens, Message, {tokenType::KEYWORD}, 0, true, "INT");
    if(checkType.first){
        auto checkSize = validTokenAt(i+2, tokens, Message, {tokenType::INT}, 1, true, 4);
        if(!checkSize.first){
            Message += "\n  : INT size is always 4.";
            return false;
        }
        curCols[colName] = {TypeId::INT32, 4, pos_i};
        return true;
    }
    checkType = validTokenAt(i+1, tokens, Message, {tokenType::KEYWORD}, 0, true, "VARCHAR");
    if(checkType.first){
        auto checkSize = validTokenAt(i+2, tokens, Message, {tokenType::INT}, 1);
        if(!checkSize.first){
            Message += "\n : VARCHAR not specified correctly.";
            return false;
        }
        uint32_t colSize = std::get<1>(checkSize.second);
        if(colSize < 1 || colSize  >= 256){
            Message = "VARCHAR column size should be in [1,255] bytes size.";
            return false;
        }
        curCols[colName] = {TypeId::VARCHAR, colSize, pos_i};
        return true;
    }
    Message = "Invalid  Type specifier.";
    return false;
}

std::pair<bool, Value> checkVal(Token &tok, std::string &Message, Schema *&schema, std::string &colNm)
{
    // checks if a token is valid as a value for a column
    // fetch column first
    std::vector<std::string> colNames = schema->getColNames();
    uint32_t ci = colNames.size();
    for(uint32_t i = 0; i<colNames.size();i++)
    {
        if(colNames[i] == colNm)
        {
            ci = i;
            break;
        }
    }
    if(ci == colNames.size())
    {
        Message = "Column name is not found.";
        return {false, Value(0)};
    }
    const Column col = schema->GetColumn(ci);
    if(col.type == TypeId::INT32)
    {
        if(tok.type == tokenType::INT)
        {
            int32_t num = std::get<1>(tok.value);
            return {true, Value(num)};
        }
        Message = "Expected INT Token.";
        return {false,Value(0)};
    }
    else if(col.type == TypeId::VARCHAR)
    {
        if(tok.type == tokenType::FORCE_STR)
        {
            std::string str = std::get<0>(tok.value);
            if(str.size() + 1 > col.length)
            {
                Message = "String is too large for column.";
                return {false,Value(0)};
            }
            return {true, Value(str)};
        }
        Message = "Expected QUOTED_STR Token.";
        return {false,Value(0)};
    }
    Message = "Undefined error near SET.";
    return {false, Value(0)};
}

#endif
