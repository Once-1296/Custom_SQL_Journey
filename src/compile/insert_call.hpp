#ifndef INSERT_CALL_HPP
#define INSERT_CALL_HPP

#include "../custom_catalog.hpp"
#include "command_validation_helpers.hpp"
#include "../types/Token.hpp"
#include <vector>

std::pair<bool, std::vector<Value>> isValidRow(std::vector<Token> &tokens, std::string &Message, Schema *&tableSchema, uint32_t &start_i)
{
    uint32_t rowSize = tableSchema->GetColumnCount();
    uint32_t tokenCount = 2 * rowSize + 1;
    if (start_i + tokenCount > tokens.size())
    {
        Message = "Incomplete row.";
        return {false, {}};
    }
    std::vector<Value> row = {};
    for (uint32_t i = start_i, col_i = 0; i < start_i + tokenCount; i++)
    {
        if (i == start_i)
        {
            auto check_OB = validTokenAt(i, tokens, Message, {tokenType::BRACKET_OPEN});
            if (!check_OB.first)
            {
                Message += "\n : Invalid token instead of Opening bracket (";
                return {false, {}};
            }
        }
        else if (i + 1 == start_i + tokenCount)
        {
            auto check_CB = validTokenAt(i, tokens, Message, {tokenType::BRACKET_CLOSE});
            if (!check_CB.first)
            {
                Message += "\n : Invalid token instead of Closing bracket )";
                return {false, {}};
            }
        }
        else if ((i - start_i) % 2 == 0)
        {
            auto isComma = validTokenAt(i, tokens, Message, {tokenType::COMMA});
            if (!isComma.first)
            {
                Message += "\n : Invalid token near expected comma.";
                return {false, {}};
            }
        }
        else
        {
            Column col = tableSchema->GetColumn(col_i);
            if (col.type == TypeId::INT32)
            {
                if (tokens[i].type != tokenType::INT)
                {
                    Message = "Expected INT token but got " + printType(tokens[i].type) + " token.";
                    return {false, {}};
                }
                row.push_back(Value(int32_t(std::get<1>(tokens[i].value))));
            }
            else
            {
                if (tokens[i].type != tokenType::FORCE_STR)
                {
                    Message = "Expected QUOTED STR token but got " + printType(tokens[i].type) + " token.";
                    return {false, {}};
                }
                if (std::get<0>(tokens[i].value).size() + 1 > col.length)
                {
                    Message = "String is too large for Column. Size limit is " + std::to_string(col.length);
                    return {false, {}};
                }
                row.push_back(Value(std::get<0>(tokens[i].value)));
            }
            col_i += 1;
        }
    }
    start_i += tokenCount;
    return {true, row};
}

bool insertRows(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (tokens.size() <= 4)
    {
        Message = "Incomplete Insert command.";
        return false;
    }
    if (cata == nullptr)
    {
        Message = "Database is not linked.";
        return false;
    }
    auto check_INSERT = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "INSERT");
    if (!check_INSERT.first)
    {
        Message += "\n : Invalid token instead of INSERT keyword";
        return false;
    }
    auto check_INTO = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "INTO");
    if (!check_INTO.first)
    {
        Message += "\n : Invalid token instead of INTO keyword";
        return false;
    }
    auto check_TBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for Table Name.";
        return false;
    }
    auto check_VALUES = validTokenAt(3, tokens, Message, {tokenType::KEYWORD}, 0, true, "VALUES");
    if (!check_VALUES.first)
    {
        Message += "\n : Invalid token instead of VALUES keyword";
        return false;
    }
    std::string tableName = std::get<0>(check_TBName.second);
    Schema *schema = cata->GetTableSchema(tableName);
    if (schema == nullptr)
    {
        Message = "Table does not exist.";
        return false;
    }
    std::vector<std::vector<Value>> rowsToInsert;
    for (uint32_t i = 4;;)
    {
        auto tryRowInsert = isValidRow(tokens, Message, schema, i);
        if (!tryRowInsert.first)
        {
            Message += "\n : Some row is invalid.";
            return false;
        }
        rowsToInsert.push_back(tryRowInsert.second);
        if (i == tokens.size())
        {
            break;
        }
        auto isComma = validTokenAt(i, tokens, Message, {tokenType::COMMA});
        if (!isComma.first)
        {
            Message += "\n : Invalid token near expected comma.";
            return false;
        }
        i++;
    }
    if(rowsToInsert.empty()){
        Message = "No rows given.";
        return false;
    }
    int32_t change_count;
    cata->InsertRow(tableName, rowsToInsert, &change_count);
    std::cout<<"Inserted "<<change_count<<" rows successfully."<<std::endl;
    return true;
}

#endif