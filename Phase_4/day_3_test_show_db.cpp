#include <iostream>
#include <cassert>
#include "parser.hpp"
#include "command_interpreter.hpp"
#include "command_caller.hpp"
int main() {
    Caller caller;
    Token root(TokenType::ROOT);
    std::string qry = "SHOW DATABASES;";
    Parser parser;
    parser.parseIntoTokens(qry, root);
    assert(identifyCommand(root)==true);
    caller.showDBs();
    return 0;
}