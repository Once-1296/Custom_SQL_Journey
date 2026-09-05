#include <iostream>
#include <cassert>
#include <vector>
#include "lexer.hpp"

void runTest(lexer& lex, std::string query, const std::string& testName) {
    std::vector<Token> tokens;
    std::string message;
    bool success = lex.lex(query, tokens, message);
    
    if (!success) {
        std::cout << testName << " FAILED: " << message << std::endl;
    }
    assert(success == true);
    std::cout << testName << " completed !" << std::endl;
    lex.printTokens(tokens);
    std::cout << "\n-------------------------------------------------------\n";
}

int main() {
    lexer lex;
    
    std::cout << "=======================================================\n";
    std::cout << "                 POSITIVE TESTS START                  \n";
    std::cout << "=======================================================\n\n";

    // Original 13 Tests (Adapted * to ALL)
    runTest(lex, "SELECT ALL FROM \"table 1\" WHERE (age > 30 AND name = \"Awwab\");", "Parsing Test 1");
    runTest(lex, "SHOW DATABASES;", "Parsing Test 2");
    runTest(lex, "SHOW DATABASE \"Students\";", "Parsing Test 3");
    runTest(lex, "LINK DATABASE \"Hotels\";", "Parsing Test 4");
    runTest(lex, "UNLINK DATABASE \"Hotels\";", "Parsing Test 5");
    runTest(lex, "SHOW TABLE \"Marks\";", "Parsing Test 6");
    runTest(lex, "SHOW TABLE SCHEMA \"Marks\";", "Parsing Test 7");
    runTest(lex, "DELETE TABLE  \"Marks\";", "Parsing Test 8");
    runTest(lex, "CREATE TABLE Jobs ( ID INT 4, SALARY INT 4, COMPANY_NAME VARCHAR 20, PRIMARY KEY (ID) );", "Parsing Test 9");
    runTest(lex, "INSERT INTO \"Jobs\" VALUES ((0,10000,\"DE SHAW\"), (1,15000,\"GS\"),(2,10,\"Infosys\"));", "Parsing Test 10");
    runTest(lex, "SELECT name AS  \"company name\", salary AS tankhua FROM \"Jobs\" WHERE salary > 1000;", "Parsing Test 11");
    runTest(lex, "UPDATE Jobs SET salary = 1 WHERE name = \"Infosys\";", "Parsing Test 12");
    runTest(lex, "DELETE FROM \"Jobs\" WHERE salary > 0;", "Parsing Test 13");

    // 2 New Tests for Lexer Features
    runTest(lex, "SELECT ALL FROM \"Users\" WHERE name STARTSWITH \"A\";", "New Test 14 (String Operators)");
    runTest(lex, "UPDATE \"Stats\" SET score = (100 + 2) * (4 - 9);", "New Test 15 (Math Operators & Brackets)");

    std::cout << "                   ALL POSITIVE TESTS PASSED           \n";
    std::cout << "=======================================================\n";

    return 0;
}