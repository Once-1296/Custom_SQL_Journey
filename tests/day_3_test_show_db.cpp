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
    std::string qry = "SHOW DATABASES;";
    assert(compiler.query(qry) == true);
    return 0;
}