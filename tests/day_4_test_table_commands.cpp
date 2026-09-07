#include <iostream>
#include <cassert>
#include "../src/compiler.hpp"
int main(int argc, char* argv[]) {
    std::string p = ".";
    if(argc > 1){
        char* path = argv[1];
        p = path;
    }
    std::cout<<p<<std::endl;
    Compiler compiler(p);
    std::string qry ="CREATE DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    qry = "SHOW DATABASES;";
    assert(compiler.query(qry) == true);
    qry = "LINK DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLES;";
    assert(compiler.query(qry) == true);
    qry = "CREATE TABLE Student ( ID INT 4, Age INT 4, Name VARCHAR 20, PRIMARY KEY (ID));";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLES;";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLE SCHEMA \"Student\";";
    assert(compiler.query(qry) == true);
    qry = "CREATE TABLE Items ( \"Item Type\" VARCHAR 5, Price INT 4, \"Item Name\" VARCHAR 20, \"Company Name\" VARCHAR 30 ,PRIMARY KEY (\"Item Name\", \"Company Name\"));";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLES;";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLE SCHEMA \"Items\";";
    assert(compiler.query(qry) == true);
    qry = "DELETE TABLE \"Student\";";
    assert(compiler.query(qry) == true);
    qry = "SHOW TABLES;";
    assert(compiler.query(qry) == true);
    qry = "UNLINK DATABASE;";
    assert(compiler.query(qry) == true);
    qry = "DELETE DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    return 0;
}