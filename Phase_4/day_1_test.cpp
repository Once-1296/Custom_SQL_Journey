#include<iostream>
#include<cassert>
#include "parser.hpp"
int main(){
    
    Parser parser;
    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT * FROM \"table 1\" WHERE (age > 30 AND name = \"Awwab\");";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 1 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW DATABASES;";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 2 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW DATABASE \"Students\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 3 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "LINK DATABASE \"Hotels\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 4 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "UNLINK DATABASE \"Hotels\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 5 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW TABLE \"Marks\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 6 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW TABLE SCHEMA \"Marks\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 7 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "DELETE TABLE  \"Marks\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 8 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "CREATE TABLE Jobs ( ID INT 4, SALARY INT 4, COMPANY_NAME VARCHAR 20, PRIMARY KEY (ID) );";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 9 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "INSERT INTO \"Jobs\" VALUES ((0,10000,\"DE SHAW\"), (1,15000,\"GS\"),(2,10,\"Infosys\"));";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 10 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT name as  \"company name\", salary as tankhua FROM \"Jobs\" WHERE salary > 1000;";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 11 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "UPDATE Jobs SET salary = 1 WHERE name = \"Infosys\";";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 12 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
    std::cout<<"\n-------------------------------------------------------\n";
    {
        Token root(TokenType::ROOT);
        std::string query = "DELETE FROM \"Jobs\" WHERE salary > 0;";
        assert(parser.parseIntoTokens(query, root) == true);
        std::cout<<"Parsing Test 13 completed !"<<std::endl;
        parser.printParsedTokens(root);
    }
}