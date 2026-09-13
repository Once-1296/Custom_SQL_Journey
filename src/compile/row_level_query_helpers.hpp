#ifndef ROW_LEVEL_QUERY_HELPERS_HPP
#define ROW_LEVEL_QUERY_HELPERS_HPP
#include "../custom_catalog.hpp"
#include "command_validation_helpers.hpp"
#include "../types/Token.hpp"
#include "../types/Operator.hpp"
#include <vector>
#include <set>
#include <stack>

std::pair<bool, Schema *> fromHelper(std::vector<Token> &tokens, std::string &Message, catalog *&cata, uint32_t fromI, uint32_t end_i)
{
    if (cata == nullptr)
    {
        Message = "No Database is linked";
        return {false, nullptr};
    }
    if (end_i != fromI + 1)
    {
        Message = "Error near expected FROM";
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

std::unique_ptr<AbstractExpression> whereHelper(std::vector<Token> &tokens, std::string &Message, Schema *&schema, uint32_t start_i, uint32_t end_i)
{
    auto check_WHERE = validTokenAt(start_i, tokens, Message, {tokenType::KEYWORD}, 0, true, "WHERE");
    if (!check_WHERE.first)
    {
        Message += "\n : Invalid token instead of WHERE keyword";
        return nullptr;
    }
    std::vector<Token> postOrder;
    std::stack<std::pair<Token, std::string>> stk;
    std::vector<std::string> colNames = schema->getColNames();
    std::set<std::string> colSet(colNames.begin(), colNames.end());
    for (uint32_t i = start_i + 1; i <= end_i; i++)
    {
        Token tok = tokens[i];
        if (tok.type == tokenType::OPERATOR)
        {
            std::string op = std::get<0>(tok.value);
            while (!stk.empty() && getPriority(stk.top().second) >= getPriority(op))
            {
                postOrder.push_back(stk.top().first);
                stk.pop();
            }
            stk.push({tok, op});
        }
        else if (tok.type == tokenType::BRACKET_CLOSE)
        {
            while (!stk.empty() && stk.top().second != "(")
            {
                postOrder.push_back(stk.top().first);
                stk.pop();
            }
            if (stk.empty())
            {
                Message = "Irregular brackets.";
                return nullptr;
            }
            stk.pop();
        }
        else if (tok.type == tokenType::BRACKET_OPEN)
        {
            stk.push({tok, "("});
        }
        else if (tok.type == tokenType::FORCE_STR)
        {
            postOrder.push_back(tok);
        }
        else if (tok.type == tokenType::INT)
        {
            postOrder.push_back(tok);
        }
        else if (tok.type == tokenType::STR)
        {
            std::string val = std::get<0>(tok.value);
            if (!colSet.count(val))
            {
                Message = "Invalid token " + val + " in WHERE clause.";
                return nullptr;
            }
            postOrder.push_back(tok);
        }
        else
        {
            Message = "Invalid token type in WHERE clause.";
            return nullptr;
        }
    }
    while (!stk.empty())
    {
        if (stk.top().second == "(")
        {
            Message = "Open brackets not closed.";
            return nullptr;
        }
        postOrder.push_back(stk.top().first);
        stk.pop();
    }
    std::stack<std::pair<std::unique_ptr<AbstractExpression>, TypeId>> mainSTK;
    for (auto &tok : postOrder)
    {
        if (tok.type == tokenType::OPERATOR)
        {
            std::string op = std::get<0>(tok.value);
            bool isPos = processOP(mainSTK, op, Message);
            if (!isPos)
            {
                Message += "Failed to parse WHERE clause.";
                return nullptr;
            }
        }
        else if (tok.type == tokenType::FORCE_STR)
        {
            std::string str = std::get<0>(tok.value);
            std::unique_ptr<ConstantValueExpression> uptr = std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(str)));
            mainSTK.push({std::move(uptr), TypeId::VARCHAR});
        }
        else if (tok.type == tokenType::INT)
        {
            int32_t num = std::get<1>(tok.value);
            std::unique_ptr<ConstantValueExpression> uptr = std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(num)));
            mainSTK.push({std::move(uptr), TypeId::INT32});
        }
        else
        {
            std::string cname = std::get<0>(tok.value);
            uint32_t ind = 0;
            TypeId type = TypeId::INT32;
            for (uint32_t i = 0; i < colNames.size(); i++)
            {
                if (colNames[i] == cname)
                {
                    type = schema->GetColumn(i).type;
                    ind = i;
                    break;
                }
            }
            std::unique_ptr<ColumnValueExpression> uptr = std::make_unique<ColumnValueExpression>(ColumnValueExpression(ind));
            mainSTK.push({std::move(uptr), type});
        }
    }
    if (mainSTK.empty())
    {
        Message = "No condition provided in WHERE clause.";
        return nullptr;
    }
    if (mainSTK.size() > 1)
    {
        Message = "Improper WHERE clause.";
        return nullptr;
    }
    return std::move(mainSTK.top().first);
}

std::vector<std::pair<uint32_t, uint32_t>> orderByHelper(std::vector<Token> &tokens, std::string &Message, Schema *&outSchema, uint32_t start_i, uint32_t end_i)
{
    auto check_ORDERBY = validTokenAt(start_i, tokens, Message, {tokenType::KEYWORD}, 0, true, "ORDERBY");
    if (!check_ORDERBY.first)
    {
        Message += "\n : Invalid token instead of ORDERBY keyword";
        return {};
    }
    std::vector<Token> col;
    std::set<std::string> selectedCols;
    std::vector<std::string> colNames = outSchema->getColNames();
    std::map<std::string, uint32_t> colNameMap;
    std::vector<std::pair<uint32_t, uint32_t>> order;
    for (uint32_t i = 0; i < colNames.size(); i++)
    {
        colNameMap[colNames[i]] = i;
    }
    auto isValidCol = [&col, &selectedCols, &colNameMap, &order]() -> bool
    {
        if (col.size() != 1 && col.size() != 2)
        {
            return false;
        }
        if (col[0].type != tokenType::STR && col[0].type != tokenType::FORCE_STR)
        {
            return false;
        }
        std::string colNm = std::get<0>(col[0].value);
        if (!colNameMap.contains(colNm))
            return false;
        if (selectedCols.contains(colNm))
            return false;
        uint32_t ord = 0; // default ascending
        if (col.size() == 2)
        {
            if (col[1].type != tokenType::KEYWORD)
            {
                return false;
            }
            else if (std::get<0>(col[1].value) == "ASC")
            {
                ord = 0;
            }
            if (std::get<0>(col[1].value) == "DESC")
            {
                ord = 1;
            }
            else
            {
                return false;
            }
        }
        selectedCols.insert(colNm);
        order.push_back({colNameMap[colNm], ord});
        col.clear();
        return true;
    };
    for (uint32_t i = start_i + 1; i <= end_i; i++)
    {
        if (tokens[i].type == tokenType::COMMA)
        {
            bool ok = isValidCol();
            if (!ok)
            {
                Message = "Some column in orderby is wrong.";
                return {};
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
        Message = "Some column in orderby is wrong/Empty Select.";
        return {};
    }
    return order;
}

bool limitHelper(std::vector<Token> &tokens, std::string &Message, std::vector<Tuple> &rows, uint32_t start_i, uint32_t end_i)
{
    if (end_i != start_i + 1)
    {
        Message = "Error near expected LIMIT";
        return false;
    }
    auto check_LIMIT = validTokenAt(start_i, tokens, Message, {tokenType::KEYWORD}, 0, true, "LIMIT");
    if (!check_LIMIT.first)
    {
        Message += "\n : Invalid token instead of LIMIT keyword";
        return false;
    }
    auto check_NUM = validTokenAt(end_i, tokens, Message, {tokenType::INT}, 1);
    if (!check_NUM.first)
    {
        Message += "\n : Expected integer in LIMIT";
        return false;
    }
    int32_t cnt = std::get<1>(check_NUM.second);
    if (cnt <= 0)
    {
        Message = "Expected positive integer after LIMIT.";
        return false;
    }
    while (rows.size() > cnt)
    {
        rows.pop_back();
    }
    return true;
}

std::vector<std::pair<std::string, Value>> setHelper(std::vector<Token> &tokens, std::string &Message, Schema *&schema, uint32_t start_i, uint32_t end_i)
{
    auto check_SET = validTokenAt(start_i, tokens, Message, {tokenType::KEYWORD}, 0, true, "SET");
    if (!check_SET.first)
    {
        Message += "\n : Invalid token instead of SET keyword";
        return {};
    }
    std::vector<std::string> allowedCols = schema->getNonPrimaryColNames();

    std::set<std::string> colNameSet(allowedCols.begin(), allowedCols.end());
    std::set<std::string> selectedCols;
    std::vector<std::pair<std::string, Value>> updCols;
    std::vector<Token> col;
    auto isValidCol = [&col, &selectedCols, &colNameSet, &updCols, &schema]() -> bool
    {
        if (col.size() != 3)
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

        selectedCols.insert(colNm);
        std::string rnds;
        auto checkEquals = validTokenAt(1, col, rnds, {tokenType::OPERATOR}, 0, true, "=");
        if (!checkEquals.first)
        {
            return false;
        }
        auto val = checkVal(col[2], rnds, schema, colNm);
        if (!val.first)
        {
            return false;
        }
        updCols.push_back({colNm, val.second});
        col.clear();
        return true;
    };
    for (uint32_t i = start_i + 1; i <= end_i; i++)
    {
        if (tokens[i].type == tokenType::COMMA)
        {
            bool ok = isValidCol();
            if (!ok)
            {
                Message = "Some column in SET is wrong.";
                return {};
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
        Message = "Some column in SET is wrong/Empty SET.";
        return {};
    }
    return updCols;
}

#endif