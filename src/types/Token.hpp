#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <iostream>
#include <variant>
const char *KEYWORDS[] = {
    "SHOW", "CREATE", "LINK", "UNLINK", "DELETE",
    "SCHEMA", "PRIMARY", "KEY",
    "TABLE", "TABLES", "DATABASE", "DATABASES",
    "INSERT", "INTO", "UPDATE", "SET",
    "SELECT", "FROM", "WHERE", "AS",
    "ALL", "VALUES", "INT", "VARCHAR",
    "ORDERBY", "LIMIT", "ASC", "DESC"
};

const char* OPERATORS[] = {
    ">", "<", "=",">=", "<=",
    "!=", "STARTSWITH", "ENDSWITH",
    "+", "-", "*", "/", "!", "SUB",
    "AND", "OR", "XOR", "NOT",
    "&", "^", "|", "~"
};
enum class tokenType{
    STR,
    INT,
    KEYWORD,
    OPERATOR,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    COMMA,
    FORCE_STR
};
struct Token{
    tokenType type;
    std::variant<std::string,int> value;
};

std::string printType(tokenType type){
    std::string res = "";
    switch (type)
    {
    case tokenType::STR:
        res  = "STR";
        break;
    case tokenType::INT:
        res  = "INT";
        break;
    case tokenType::KEYWORD:
        res  = "KEYWORD";
        break;
    case tokenType::OPERATOR:
        res  = "OPERATOR";
        break;
    case tokenType::BRACKET_OPEN:
        res  = "BRACKET_OPEN";
        break;
    case tokenType::BRACKET_CLOSE:
        res  = "BRACKET_CLOSE";
        break;
    case tokenType::COMMA:
        res  = "COMMA";
        break;
    case tokenType::FORCE_STR:
        res  = "QUOTED STR";
        break;
    default:
        res  = "INVALID";
        break;
    }
    return res;
}
#endif