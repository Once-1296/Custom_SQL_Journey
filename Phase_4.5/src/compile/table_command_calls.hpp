#ifndef TABLE_COMMAND_CALLS_HPP
#define TABLE_COMMAND_CALLS_HPP

#include "../custom_catalog.hpp"
#include "command_validation_helpers.hpp"

bool showTBs(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (tokens.size() != 2)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (cata == nullptr)
    {
        Message = "No database is linked";
        return false;
    }
    auto check_SHOW = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "SHOW");
    if (!check_SHOW.first)
    {
        Message += "\n : Invalid token instead of SHOW keyword";
        return false;
    }
    auto check_TBS = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "TABLES");
    if (!check_TBS.first)
    {
        Message += "\n : Invalid token instead of TABLES keyword";
        return false;
    }
    std::cout << "Tables of " << cata->getDBName() << " Database" << std::endl;
    print_table(tab_schema, cata->GetDBMeta());
    return true;
}

bool showTBSchema(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (tokens.size() != 4)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (cata == nullptr)
    {
        Message = "No database is linked";
        return false;
    }
    auto check_SHOW = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "SHOW");
    if (!check_SHOW.first)
    {
        Message += "\n : Invalid token instead of SHOW keyword";
        return false;
    }
    auto check_TB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "TABLE");
    if (!check_TB.first)
    {
        Message += "\n : Invalid token instead of TABLE keyword";
        return false;
    }
    auto check_SC = validTokenAt(2, tokens, Message, {tokenType::KEYWORD}, 0, true, "SCHEMA");
    if (!check_SC.first)
    {
        Message += "\n : Invalid token instead of SCHEMA keyword";
        return false;
    }
    auto check_TBName = validTokenAt(3, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for Table Name.";
        return false;
    }
    std::string tableName = std::get<0>(check_TBName.second);
    auto res = cata->GetSchemaCols(tableName);
    if (std::get<0>(res) == false)
    {
        Message = "Table does not exist.";
        return false;
    }
    std::cout << "Columns of " << tableName << " table" << std::endl;
    print_table(col_schema, std::get<2>(res), true);
    return true;
}
bool delTB(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (cata == nullptr)
    {
        Message = "Not linked to a Database.";
        return false;
    }
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    auto check_DEL = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "DELETE");
    if (!check_DEL.first)
    {
        Message += "\n : Invalid token instead of DELETE keyword";
        return false;
    }
    auto check_TB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "TABLE");
    if (!check_TB.first)
    {
        Message += "\n : Invalid token instead of TABLE keyword";
        return false;
    }
    auto check_TBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for Table Name.";
        return false;
    }
    std::string tableName = std::get<0>(check_TBName.second);
    auto res = cata->DeleteTable(tableName);
    if (!res)
    {
        Message = "Table does not exist in database: " + cata->getDBName() + " .";
    }
    std::cout << "Successfully deleted table " + tableName + "." << std::endl;
    return res;
}

bool createTB(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (cata == nullptr)
    {
        Message = "No database is linked";
        return false;
    }
    if (tokens.size() <= 5)
    {
        Message = "Incomplete tokens.";
        return false;
    }
    auto check_CREATE = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "CREATE");
    if (!check_CREATE.first)
    {
        Message += "\n : Invalid token instead of CREATE keyword";
        return false;
    }
    auto check_TB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "TABLE");
    if (!check_TB.first)
    {
        Message += "\n : Invalid token instead of TABLE keyword";
        return false;
    }
    auto check_TBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_TBName.first)
    {
        Message += "\n : Invalid token for DB Name.";
        return false;
    }
    std::string tableName = std::get<0>(check_TBName.second);
    bool validName = isValidTableName(tableName, Message);
    if (!validName)
    {
        Message += "\n : Invalid Table Name";
        return false;
    }
    bool exists = cata->GetTableSchema(tableName) != nullptr;
    if (exists)
    {
        Message = "Table " + tableName + " already exists.";
        return false;
    }
    auto check_OB = validTokenAt(3, tokens, Message, {tokenType::BRACKET_OPEN});
    if (!check_OB.first)
    {
        Message += "\n : Invalid token instead of Opening bracket (";
        return false;
    }
    auto check_CB = validTokenAt(tokens.size() - 1, tokens, Message, {tokenType::BRACKET_CLOSE});
    if (!check_CB.first)
    {
        Message += "\n : Invalid token instead of CLosing bracket )";
        return false;
    }
    uint32_t PKind = -1;
    std::map<std::string, std::tuple<TypeId, uint32_t, uint32_t>> curCols;
    uint32_t col_i = 0;
    for (uint32_t i = 4; i + 3 < tokens.size(); i += 4)
    {
        auto isColName = validTokenAt(i, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
        if (isColName.first)
        {
            bool success = checkCol(i, tokens, Message, curCols, col_i);
            if (!success)
            {
                return false;
            }
            auto isComma = validTokenAt(i + 3, tokens, Message, {tokenType::COMMA});
            if (!isComma.first)
            {
                Message += "\n : Invalid token near expected comma.";
                return false;
            }
            col_i++;
            continue;
        }
        auto isPrimKey = validTokenAt(i, tokens, Message, {tokenType::KEYWORD}, 0, true, "PRIMARY");
        if (isPrimKey.first)
        {
            PKind = i;
            break;
        }
        Message = "Invalid token at " + std::to_string(i) + "th position.";
        return false;
    }
    if (PKind == -1)
    {
        Message = "Primary key not found.";
        return false;
    }
    if(curCols.empty()){
        Message = "No columns found.";
        return false;
    }
    auto isKey = validTokenAt(PKind + 1, tokens, Message, {tokenType::KEYWORD}, 0, true, "KEY");
    if (!isKey.first)
    {
        Message += "\n : KEY follows PRIMARY.";
        return false;
    }
    std::set<std::string> primaryKeys;
    for (uint32_t i = 0; i + PKind + 2 < tokens.size() - 1; i++)
    {
        uint32_t ind = PKind + 2 + i;
        if (i == 0)
        {
            check_OB = validTokenAt(ind, tokens, Message, {tokenType::BRACKET_OPEN});
            if (!check_OB.first)
            {
                Message += "\n : Invalid token instead of Opening bracket (";
                return false;
            }
        }
        else if (i + PKind + 2 == tokens.size() - 2)
        {
            check_CB = validTokenAt(ind, tokens, Message, {tokenType::BRACKET_CLOSE});
            if (!check_CB.first)
            {
                Message += "\n : Invalid token instead of Closing bracket )";
                return false;
            }
        }
        else if (i % 2 == 0)
        {
            auto isComma = validTokenAt(ind, tokens, Message, {tokenType::COMMA});
            if (!isComma.first)
            {
                Message += "\n : Invalid token near expected comma.";
                return false;
            }
        }
        else
        {
            auto isColName = validTokenAt(ind, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
            if (!isColName.first)
            {
                Message += "\n : Invalid column name token in primary key.";
                return false;
            }
            std::string pKColName = std::get<0>(isColName.second);
            if (curCols.find(pKColName) == curCols.end())
            {
                Message = "Invalid column name token in primary key.";
                return false;
            }
            if (primaryKeys.find(pKColName) != primaryKeys.end())
            {
                Message = "Repeated primary key : " + pKColName;
                return false;
            }
            primaryKeys.insert(pKColName);
        }
    }
    if (primaryKeys.empty())
    {
        Message = "Empty Primary key not allowed.";
        return false;
    }
    Column dummy(const_cast<char *>("xoxo"),TypeId::VARCHAR,0,0,0);
    std::vector<Column> cols(col_i, dummy);
    for(auto&it:curCols){
        char *cname = new char[it.first.size() + 1];
        std::strncpy(cname, it.first.c_str(), it.first.size());
        cname[it.first.size()] = '\0';
        TypeId ctype = std::get<0>(it.second);
        uint32_t clength = std::get<1>(it.second);
        uint32_t coffset = 0;
        bool c_is_candidate_key = (primaryKeys.contains(it.first));
        Column col(cname, ctype, clength, coffset, c_is_candidate_key);
        cols[std::get<2>(it.second)] = col;
    }
    Schema schema(cols);
    cata->createTable(tableName, schema);
    std::cout<<"Successfully created table "<<tableName<<std::endl;
    return true;
}
#endif