#ifndef COMPILER_HPP
#define COMPILER_HPP
#include<iostream>
#include<dirent.h>
#include "custom_catalog.hpp"
#include "compile/parser.hpp"
#include "compile/command_interpreter.hpp"
#include "compile/command_calls.hpp"
class Compiler{
    private:
    DIR *dir;
    struct dirent *entry;
    std::string root_path = ".";
    catalog *cata = nullptr;
    Parser parser;
    public:
    Compiler(std::string path = "."):root_path(path){

    }
    bool query(std::string &query){
        Token root(TokenType::ROOT);
        bool parsedSuccessfully = parser.parseIntoTokens(query, root);
        if(!parsedSuccessfully)return false;
        std::string Message;
        int command = identifyCommand(root, Message);
        if(command == -1){
            std::cout<<"Error Found: "<<Message<<std::endl;
            return false;
        }
        if(command == 0){
            showDBs(dir,entry, root_path);
            return true;
        }
        return false;
    }
};

#endif