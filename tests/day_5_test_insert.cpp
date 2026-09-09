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

        // test single insert
        qry = "INSERT INTO Student VALUES (1, 18, \"Awwab\");";
        assert(compiler.query(qry) == true);
        std::cout << "ayo what\n";
    }
    {
        // verify
        catalog cata(p + "/awwab.db");
        std::vector<std::string> cols = {"ID", "Age", "Name"};
        auto res = cata.Query("Student", cols);
        assert(std::get<0>(res) == true);
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        // test multiple insert
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (2, 19, \"John\"), (3, 20, \"Doe\");";
        assert(compiler.query(qry) == true);
    }
    {
        // verify
        catalog cata(p + "/awwab.db");
        std::vector<std::string> cols = {"ID", "Age", "Name"};
        auto res = cata.Query("Student", cols);
        assert(std::get<0>(res) == true);
        print_table(std::get<1>(res), std::get<2>(res));
    }

    {
        // test insert with wrong number of values
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (4, 21);";
        assert(compiler.query(qry) == false);
    }
    {
        // test insert with wrong data type
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (5, \"twenty\", \"Jane\");";
        assert(compiler.query(qry) == false);
    }
    {
        // test insert with string too long
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (6, 22, \"This name is way too long for the column\");";
        assert(compiler.query(qry) == false);
    }
    {
        // test repeat insert (should succeeed with 0 rows inserted)
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (1, 18, \"Awwab\");";
        assert(compiler.query(qry) == true);
    }
    {
        // verify
        catalog cata(p + "/awwab.db");
        std::vector<std::string> cols = {"ID", "Age", "Name"};
        auto res = cata.Query("Student", cols);
        assert(std::get<0>(res) == true);
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        // test repeat insert + duplicate insert
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES (7, 19, \"Aadil\"),(1, 18, \"Awwab\"), (7, 21, \"Aadil\"), (12, 20, \"Aribah\");";
        // should be 2 new rows inserted only
        assert(compiler.query(qry) == true);
    }
    {
        // verify
        catalog cata(p + "/awwab.db");
        std::vector<std::string> cols = {"ID", "Age", "Name"};
        auto res = cata.Query("Student", cols);
        assert(std::get<0>(res) == true);
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        // test large number of inserts
        Compiler compiler(p);
        std::string qry = "LINK DATABASE \"awwab\";";
        assert(compiler.query(qry) == true);
        qry = "INSERT INTO Student VALUES ";
        for (int i = 4; i <= 1003; ++i)
        {
            qry += "(" + std::to_string(i) + ", " + std::to_string(17 + (i % 10)) + ", \"Student" + std::to_string(i) + "\")";
            if (i != 1003)
            {
                qry += ", ";
            }
        }
        qry += ";";
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