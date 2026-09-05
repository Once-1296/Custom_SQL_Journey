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
    "ALL", "VALUES", "INT", "VARCHAR"
};

const char* OPERATORS[] = {
    ">", "<", "=",">=", "<=",
    "!=", "STARTSWITH", "ENDSWITH",
    "+", "-", "*", "/", "!", 
    "AND", "OR"
};
enum tokenType{
    STR,
    INT,
    KEYWORD,
    OPERATOR,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    COMMA,
};
struct Token{
    tokenType type;
    std::variant<std::string,int> value;
};
#endif