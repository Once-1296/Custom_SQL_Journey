#include<iostream>
#include "parser.hpp"
int main(){
    
    Parser parser;
    Token root(TokenType::ROOT);
    std::string query = "SELECT * FROM table1 WHERE (age > 30 AND name = \"Awwab\");";
    parser.parseIntoTokens(query, root);
    std::cout<<"Parsing completed !"<<std::endl;
    parser.printParsedTokens(root);
}