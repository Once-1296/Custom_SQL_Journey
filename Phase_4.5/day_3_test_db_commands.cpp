#include <iostream>
#include <cassert>
#include "compiler.hpp"
int main(int argc, char* argv[]) {
    std::string p = ".";
    if(argc > 1){
        char* path = argv[1];
        p = path;
    }
    std::cout<<p<<std::endl;
    Compiler compiler(p);
    std::string qry = "SHOW DATABASES;";
    assert(compiler.query(qry) == true);
    qry = "CREATE DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    assert(compiler.query(qry) == false);
    qry = "SHOW DATABASES;";
    assert(compiler.query(qry) == true);
    qry = "LINK DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    assert(compiler.query(qry) == false);
    qry = "UNLINK DATABASE;";
    assert(compiler.query(qry) == true);
    assert(compiler.query(qry) == false);
    qry = "DELETE DATABASE \"awwab\";";
    assert(compiler.query(qry) == true);
    assert(compiler.query(qry) == false);
    return 0;
}