#ifndef ROW_LEVEL_QUERY_HELPERS_HPP
#define ROW_LEVEL_QUERY_HELPERS_HPP
#include "../custom_catalog.hpp"
#include "command_validation_helpers.hpp"
#include "../types/Token.hpp"
#include <vector>
#include <set>

std::pair<bool, Schema *> fromHelper(std::vector<Token> &tokens, std::string &Message, catalog *&cata, uint32_t fromI)
{
    if (cata == nullptr)
    {
        Message = "No Database is linked";
        return {false, nullptr};
    }
    auto check_FROM = validTokenAt(fromI, tokens, Message, {tokenType::KEYWORD}, 0, true, "FROM");
    if (!check_FROM.first)
    {
        Message += "\n : Invalid token instead of FROM keyword";
        return {false, nullptr};
    }
    auto check_TBName = validTokenAt(fromI + 1, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for Table Name.";
        return {false, nullptr};
    }
    std::string tableName = std::get<0>(check_TBName.second);
    Schema *schema = cata->GetTableSchema(tableName);
    if (schema == nullptr)
    {
        Message = "Table does not exist.";
        return {false, nullptr};
    }
    return {true, schema};
}

bool selectHelper(std::vector<Token> &tokens, std::string &Message, catalog *&cata, Schema *&schema, uint32_t endI, std::vector<std::string> &qryCols, std::vector<std::string> &outCols, uint32_t selectI = 0)
{
    if (cata == nullptr)
    {
        Message = "No Database is linked";
        return false;
    }
    auto check_SELECT = validTokenAt(selectI, tokens, Message, {tokenType::KEYWORD}, 0, true, "SELECT");
    if (!check_SELECT.first)
    {
        Message += "\n : Invalid token instead of SELECT keyword";
        return false;
    }
    if (selectI + 1 == endI)
    {
        auto check_ALL = validTokenAt(endI, tokens, Message, {tokenType::KEYWORD}, 0, true, "ALL");
        if (check_ALL.first)
        {
            qryCols.clear(), outCols.clear();
            qryCols = schema->getColNames(), outCols = qryCols;
            return true;
        }
    }
    qryCols.clear(), outCols.clear();
    std::vector<Token> col;
    std::map<std::string, std::string> selectedCols;
    std::vector<std::string> colNames = schema->getColNames();
    std::set<std::string> colNameSet(colNames.begin(), colNames.end());
    auto isValidCol = [&col, &selectedCols, &colNameSet, &qryCols, &outCols]() -> bool
    {
        if (col.size() != 1 && col.size() != 3)
        {
            return false;
        }
        if (col[0].type != tokenType::STR && col[0].type != tokenType::FORCE_STR)
        {
            return false;
        }
        std::string colNm = std::get<0>(col[0].value);
        if (!colNameSet.contains(colNm))
            return false;
        if (selectedCols.contains(colNm))
            return false;
        std::string colVal = colNm;
        if (col.size() == 3)
        {
            if (col[1].type != tokenType::KEYWORD)
            {
                return false;
            }
            if (std::get<0>(col[1].value) != "AS")
            {
                return false;
            }
            if (col[2].type != tokenType::STR && col[2].type != tokenType::FORCE_STR)
            {
                return false;
            }
            colVal = std::get<0>(col[2].value);
        }
        std::string rnds;
        if (!isValidColName(colVal, rnds))
        {
            return false;
        }
        selectedCols[colNm] = colVal;
        qryCols.push_back(colNm);
        outCols.push_back(colVal);
        col.clear();
        return true;
    };
    for (uint32_t i = selectI + 1; i <= endI; i++)
    {
        if (tokens[i].type == tokenType::COMMA)
        {
            bool ok = isValidCol();
            if (!ok)
            {
                Message = "Some column in select is wrong.";
                return false;
            }
        }
        else
        {
            col.push_back(tokens[i]);
        }
    }
    bool ok = isValidCol();
    if (!ok)
    {
        Message = "Some column in select is wrong/Empty Select.";
        return false;
    }
    return true;
}

#endif