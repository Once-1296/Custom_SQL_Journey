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
        qry = "CREATE TABLE checkPrint ( longSTR VARCHAR 70, num INT 4 , PRIMARY KEY (longSTR) );";
        assert(compiler.query(qry) == true);
        // Insert values whose ordering and WHERE results are easy to distinguish.
        qry = "INSERT INTO checkPrint VALUES (\"one\", 10), (\"two\", 20), (\"three\", 30), (\"four\", 40);";
        assert(compiler.query(qry) == true);

        // WHERE, ORDERBY, and LIMIT are independently optional: cover all 8 combinations.
        qry = "SELECT ALL FROM checkPrint;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint WHERE num > 15;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint ORDERBY num DESC;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint LIMIT 2;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint WHERE num > 15 ORDERBY num DESC;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint WHERE num > 15 LIMIT 2;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint ORDERBY num DESC LIMIT 2;";
        assert(compiler.query(qry) == true);
        qry = "SELECT ALL FROM checkPrint WHERE num > 15 ORDERBY num DESC LIMIT 2;";
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
