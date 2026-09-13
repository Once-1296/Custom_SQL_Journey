#ifndef UPDATE_CALL_HPP
#define UPDATE_CALL_HPP

#include "../types/Token.hpp"
#include "command_validation_helpers.hpp"
#include "../custom_catalog.hpp"
#include "row_level_query_helpers.hpp"
#include <vector>

bool updateQry(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (cata == nullptr)
    {
        Message = "No Database is linked";
        return false;
    }
    auto check_UPDATE = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "UPDATE");
    if (!check_UPDATE.first)
    {
        Message += "\n : Invalid token instead of UPDATE keyword";
        return false;
    }
    auto check_TBName = validTokenAt(1, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for Table Name.";
        return false;
    }
    std::string tableName = std::get<0>(check_TBName.second);
    Schema *schema = cata->GetTableSchema(tableName);
    if (cata == nullptr)
    {
        Message = "Table does not exist.";
        return false;
    }
    
    uint32_t end_i=tokens.size()-1;
    for(uint32_t i = 3;i<tokens.size();i++)
    {
        if(tokens[i].type == tokenType::KEYWORD)
        {
            std::string kstr = std::get<0>(tokens[i].value);
            if(kstr == "WHERE")
            {
                end_i = i - 1;
                break;
            }
        }
    }
    auto updCols = setHelper(tokens, Message, schema, 2, end_i);
    if(updCols.empty())
    {
        Message += "\n : Some error near SET/ Empty SET.";
        return false;
    }
    std::unique_ptr<AbstractExpression> whereClause = std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)));
    if(tokens.size() > end_i + 1)
    {
        std::unique_ptr<AbstractExpression> wC = std::move(whereHelper(tokens, Message, schema, end_i + 1, tokens.size()-1));
        if (wC == nullptr)
        {
            Message += "\n :  Issue in WHERE clause.";
        }
        whereClause = std::move(wC);
    }
    uint32_t upd_count;
    cata->UpdateRow(tableName,updCols, std::move(whereClause),&upd_count);
    std::cout<<"Updated "<<upd_count<<" rows successfully!"<<std::endl;
    return true;
}

#endif