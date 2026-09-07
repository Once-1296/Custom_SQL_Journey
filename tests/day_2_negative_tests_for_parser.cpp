#include <iostream>
#include <cassert>
#include "../src/compile/parser.hpp"

int main() {
    Parser parser;
    
    std::cout << "\n=======================================================\n";
    std::cout << "                 NEGATIVE TESTS START                  \n";
    std::cout << "=======================================================\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW DATABASES"; // Missing trailing semicolon
        std::cout << "Query: " << query << "\nExpected Error: Query is blank/ does not end with ;" << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 1 (Missing semicolon) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "CREATE DATABASE \"my,db\";"; // Comma inside quotes
        std::cout << "Query: " << query << "\nExpected Error: Cannot put comma in quotes." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 2 (Comma in quotes) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT* FROM Jobs;"; // * attached to alphanum
        std::cout << "Query: " << query << "\nExpected Error: Cannot put * in quotes/brackets or lack of proper space." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 3 (Asterisk spacing) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "CREATE TABLE \"Jobs(\";"; // Bracket inside quotes
        std::cout << "Query: " << query << "\nExpected Error: Cannot put bracket in quotes." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 4 (Bracket in quotes) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "INSERT INTO Jobs VALUES 1, 2, 3 );"; // Closing bracket in mode 0
        std::cout << "Query: " << query << "\nExpected Error: Closing bracket in wrong place" << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 5 (Wrong closing bracket) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SHOW TABLE\"Marks\";"; // Quote right after Alphanum
        std::cout << "Query: " << query << "\nExpected Error: Quoted content needs space before." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 6 (Missing space before quote) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT * FROM \"Jobs\" WHERE name = \"Infosys=\";"; // Operator inside quotes
        std::cout << "Query: " << query << "\nExpected Error: Operator not allowed in quotes." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 7 (Operator in quotes) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT * FROM \"Jobs;"; // Hits semicolon while mode != 0
        std::cout << "Query: " << query << "\nExpected Error: Missing closing bracket/quote." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 8 (Unclosed quotes) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "SELECT * FROM \"Jobs\"WHERE salary > 0;"; // checkNextCharacter constraint violated
        std::cout << "Query: " << query << "\nExpected Error: Missing a Space Somewhere." << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 9 (Missing space after quote) completed !" << std::endl;
    }
    std::cout << "\n-------------------------------------------------------\n";

    {
        Token root(TokenType::ROOT);
        std::string query = "CREATE TABLE my-table ( ID INT 4 );"; // "-" is not caught by isAlphaNum_
        std::cout << "Query: " << query << "\nExpected Error: Invalid character somewhere" << std::endl;
        assert(parser.parseIntoTokens(query, root) == false);
        std::cout << "Negative Test 10 (Invalid character) completed !" << std::endl;
    }
    std::cout << "\n=======================================================\n";
    std::cout << "                   ALL TESTS PASSED                    \n";
    std::cout << "=======================================================\n";

    return 0;
}