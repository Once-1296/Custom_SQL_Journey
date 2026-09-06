#include <iostream>
#include <cassert>
#include <vector>
#include "../src/compile/lexer.hpp"

void runNegativeTest(lexer& lex, std::string query, std::string expectedError, const std::string& testName) {
    std::vector<Token> tokens;
    std::string message;
    bool success = lex.lex(query, tokens, message);
    
    std::cout << "Query: " << query << "\n";
    std::cout << "Expected Error: " << expectedError << "\n";
    std::cout << "Got Error: " << message << "\n";
    
    assert(success == false);
    assert(message == expectedError);
    std::cout << testName << " completed !\n";
    std::cout << "-------------------------------------------------------\n";
}

int main() {
    lexer lex;
    
    std::cout << "\n=======================================================\n";
    std::cout << "                 NEGATIVE TESTS START                  \n";
    std::cout << "=======================================================\n\n";

    // 1. Missing Semicolon
    runNegativeTest(lex, "SHOW DATABASES", 
                    "Query is blank/ does not end with ;", 
                    "Negative Test 1 (Missing semicolon)");

    // 2. Missing Space before Quotes
    runNegativeTest(lex, "SHOW TABLE\"Marks\";", 
                    "Unterminated sequence before opening quotes.", 
                    "Negative Test 2 (Missing space before quote)");

    // 3. Unclosed Quotes (Assuming you applied the patch mentioned above)
    runNegativeTest(lex, "SELECT ALL FROM \"Jobs;", 
                    "Unclosed quotes.", 
                    "Negative Test 3 (Unclosed quotes)");

    // 4. Missing space before opening bracket
    runNegativeTest(lex, "CREATE TABLE Jobs(ID INT 4);", 
                    "Unterminated sequence before Opening Bracket.", 
                    "Negative Test 4 (Missing space before bracket)");

    // 5. Wrong closing bracket (Too many closing brackets)
    runNegativeTest(lex, "INSERT INTO Jobs VALUES (1, 2) );", 
                    "Invalid Closing Brackets.", 
                    "Negative Test 5 (Wrong closing bracket)");

    // 6. Open brackets not closed (Missing closing bracket)
    runNegativeTest(lex, "SELECT ALL FROM (Students;", 
                    "Open brackets not closed.", 
                    "Negative Test 6 (Unclosed Bracket)");

    // 7. Invalid composite operator
    runNegativeTest(lex, "SELECT ALL FROM Students WHERE age => 18;", 
                    "Invalid operator =>", 
                    "Negative Test 7 (Invalid composite operator)");

    // 8. Invalid character (using '@' since '-' is now parsed as an operator)
    runNegativeTest(lex, "CREATE TABLE my@table ( ID INT 4 );", 
                    "Invalid character.", 
                    "Negative Test 8 (Invalid character)");

    std::cout << "                   ALL NEGATIVE TESTS PASSED           \n";
    std::cout << "=======================================================\n";

    return 0;
}