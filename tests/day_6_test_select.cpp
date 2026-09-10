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
        std::cout << "ayo what\n";
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
        Compiler compiler(p);
        // clean up
        // std::string qry = "UNLINK DATABASE;";
        // assert(compiler.query(qry) == true);
        std::string qry = "DELETE DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
    }
    return 0;
}