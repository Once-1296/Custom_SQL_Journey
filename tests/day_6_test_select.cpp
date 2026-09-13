#include <iostream>
#include <cassert>
#include "../src/compiler.hpp"
int main(int argc, char *argv[])
{
    std::string p = ".";
    if (argc > 1)
    {
        char *path = argv[1];
        p = path;
    }
    std::cout << p << std::endl;
    {
        Compiler compiler(p);

        // setup
        std::string qry = "CREATE DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "CREATE TABLE Student ( ID INT 4, Age INT 4, Name VARCHAR 20, PRIMARY KEY (ID));";
        assert(compiler.query(qry) == true);

        // insert
        qry = "INSERT INTO Student VALUES (7, 19, \"Aadil\"),(1, 18, \"Awwab\"), (12, 20, \"Aribah\");";
        assert(compiler.query(qry) == true);
    }
    {
        // verify
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM Student;";
        assert(compiler.query(qry) == true);
        qry = "SELECT Name, Age FROM Student;";
        assert(compiler.query(qry) == true);
        qry = "SELECT Name AS studentName, Age, ID AS University_ID FROM Student;";
        assert(compiler.query(qry) == true);
    }
    {
        // where clause
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 1 row.
        qry = "SELECT ALL FROM Student WHERE Name STARTSWITH \"Aa\";";
        assert(compiler.query(qry) == true);
    }
    {
        // Operator test data: varied numeric, bitwise, and string values.
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "CREATE TABLE OpCases (ID INT 4, Age INT 4, Score INT 4, Bonus INT 4, Flags INT 4, FirstName VARCHAR 20, City VARCHAR 20, Code VARCHAR 20, PRIMARY KEY (ID));";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO OpCases VALUES "
              "(1, 18, 12, 5, 0, \"Aadil\", \"Delhi\", \"A1\"),"
              "(2, 21, 27, 6, 1, \"Aisha\", \"Mumbai\", \"B2\"),"
              "(3, 24, 39, 7, 2, \"Bilal\", \"Pune\", \"C3\"),"
              "(4, 28, 45, 8, 3, \"Cole\", \"Delhi\", \"D4\"),"
              "(5, 30, 51, 9, 4, \"Danish\", \"Chennai\", \"E5\"),"
              "(6, 33, 57, 10, 5, \"Eve\", \"Mumbai\", \"F6\"),"
              "(7, 36, 63, 11, 6, \"Farah\", \"Pune\", \"G7\"),"
              "(8, 39, 69, 12, 7, \"Gaurav\", \"Delhi\", \"H8\"),"
              "(9, 42, 75, 13, 0, \"Harsh\", \"Chennai\", \"I9\"),"
              "(10, 45, 81, 14, 1, \"Ira\", \"Mumbai\", \"J0\"),"
              "(11, 48, 87, 15, 2, \"Jatin\", \"Pune\", \"K1\"),"
              "(12, 51, 93, 16, 3, \"Kavya\", \"Delhi\", \"L2\"),"
              "(13, 54, 24, 17, 4, \"Lina\", \"Chennai\", \"M3\"),"
              "(14, 57, 36, 18, 5, \"Mohan\", \"Mumbai\", \"N4\"),"
              "(15, 60, 48, 19, 6, \"Nadia\", \"Pune\", \"O5\"),"
              "(16, 63, 60, 20, 7, \"Omar\", \"Delhi\", \"P6\"),"
              "(17, 66, 72, 21, 0, \"Priya\", \"Chennai\", \"Q7\"),"
              "(18, 69, 84, 22, 1, \"Rayan\", \"Mumbai\", \"R8\");";
        assert(compiler.query(qry) == true);
    }
    {
        // Relational operators and parenthesized boolean combinations.
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 7 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Score > 50) AND (Score < 80);";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 15 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Age >= 30) OR (Age <= 20);";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 14 rows.
        qry = "SELECT ALL FROM OpCases WHERE (City = \"Delhi\") XOR (Code != \"A1\");";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE NOT (Score = 0);";
        assert(compiler.query(qry) == true);
    }
    {
        // String matching, arithmetic expressions, and unary bitwise NOT.
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 2 rows.
        qry = "SELECT ALL FROM OpCases WHERE FirstName STARTSWITH \"A\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 11 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Score + 5) > 55;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 11 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Score SUB 5) >= 45;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE (~ Flags) < 0;";
        assert(compiler.query(qry) == true);
    }
    {
        // Multiplication, division, and both supported logical negations.
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Score * 2 / 2) = Score;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE - Bonus < 0;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE ! (Age = 0);";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 18 rows.
        qry = "SELECT ALL FROM OpCases WHERE NOT (Flags = 8);";
        assert(compiler.query(qry) == true);
    }
    {
        // Remaining bitwise operators and suffix matching.
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 9 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Flags & 1) = 1;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 5 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Flags | 2) = 3;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 2 rows.
        qry = "SELECT ALL FROM OpCases WHERE (Flags ^ 3) = 0;";
        assert(compiler.query(qry) == true);
        // Expected CLI output: 2 rows.
        qry = "SELECT ALL FROM OpCases WHERE Code ENDSWITH \"7\";";
        assert(compiler.query(qry) == true);
    }
    {
        Compiler compiler(p);
        // clean up
        // std::string qry = "UNLINK DATABASE;";
        // assert(compiler.query(qry) == true);
        std::string qry = "DELETE DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
    }
    return 0;
}
