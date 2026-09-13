#include <iostream>
#include <cassert>
#include "../src/compiler.hpp"

int main(int argc, char *argv[])
{
    std::string p = ".";
    if (argc > 1)
    {
        p = argv[1];
    }
    std::cout << p << std::endl;

    {
        // setup
        Compiler compiler(p);
        std::string qry = "CREATE DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "CREATE TABLE Student (ID INT 4, Age INT 4, Name VARCHAR 20, PRIMARY KEY (ID));";
        assert(compiler.query(qry) == true);
    }
    {
        // insert test data
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (1, 18, \"Awwab\"), (2, 19, \"John\"), (3, 20, \"Doe\"), (4, 22, \"Jane\"), (5, 24, \"Aadil\");";
        assert(compiler.query(qry) == true);
    }
    {
        // update rows and verify the changed table
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected: 1 row updated.
        qry = "UPDATE Student SET Age = 21 WHERE ID = 1;";
        assert(compiler.query(qry) == true);
        // Expected: 4 rows updated.
        qry = "UPDATE Student SET Name = \"Senior\" WHERE Age >= 20;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM Student ORDERBY ID;";
        assert(compiler.query(qry) == true);
    }
    {
        // delete rows and verify each resulting table state
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        // Expected: 1 row deleted.
        qry = "DELETE FROM Student WHERE Age < 20;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM Student ORDERBY ID;";
        assert(compiler.query(qry) == true);
        // Expected: 4 rows deleted.
        qry = "DELETE FROM Student;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM Student;";
        assert(compiler.query(qry) == true);
    }
    {
        // clean up
        Compiler compiler(p);
        std::string qry = "DELETE DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
    }
    return 0;
}
