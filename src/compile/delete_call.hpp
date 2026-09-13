#ifndef DELETE_CALL_HPP
#define DELETE_CALL_HPP

#include "../types/Token.hpp"
#include "command_validation_helpers.hpp"
#include "../custom_catalog.hpp"
#include "row_level_query_helpers.hpp"
#include <vector>

bool deleteQry(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (cata == nullptr)
    {
        Message = "No Database is linked";
        return false;
    }
    auto check_DELETE = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "DELETE");
    if (!check_DELETE.first)
    {
        Message += "\n : Invalid token instead of DELETE keyword";
        return false;
    }
    auto fromRes = fromHelper(tokens,Message, cata,1,2);
    if(!fromRes.first)
    {
        Message += "\n : Error in expected FROM Clause.";
        return false;
    }
    Schema *schema = fromRes.second;
    std::unique_ptr<AbstractExpression> whereClause = std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)));
    if(tokens.size() > 3)
    {
        std::unique_ptr<AbstractExpression> wC = std::move(whereHelper(tokens, Message, schema, 3, tokens.size()-1));
        if (wC == nullptr)
        {
            Message += "\n :  Issue in WHERE clause.";
        }
        whereClause = std::move(wC);
    }
    int32_t del_count;
    cata->DeleteRow(std::get<0>(tokens[2].value), std::move(whereClause),&del_count);
    std::cout<<"Deleted "<<-del_count<<" rows successfully!"<<std::endl;
    return true;
}

#endif