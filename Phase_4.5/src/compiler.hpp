#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <iostream>
#include <dirent.h>
#include "custom_catalog.hpp"
#include "compile/lexer.hpp"
#include "compile/command_interpreter.hpp"
#include "compile/db_command_calls.hpp"
#include "compile/table_command_calls.hpp"
class Compiler
{
private:
    DIR *dir;
    struct dirent *entry;
    std::string root_path = ".";
    catalog *cata = nullptr;
    lexer Lexer;

public:
    Compiler(std::string path = ".") : root_path(path)
    {
    }
    ~Compiler(){
        if(cata != nullptr){
            delete cata;
        }
    }
    bool query(std::string &query,bool debugLexer = false)
    {
        std::vector<Token> tokens;
        std::string Message;
        bool lexedSuccessfully = Lexer.lex(query, tokens, Message);
        if (!lexedSuccessfully)
        {
            std::cout << "Error found in lexer: " << Message << std::endl;
            return false;
        }
        int command = identifyCommand(tokens, Message);
        std::cout << "Command id: "<<command<<std::endl;
        if (command == -1)
        {
            std::cout << "Error found in interpreter: " << Message << std::endl;
            return false;
        }
        if (command == 0)
        {
            bool calledSuccessfully = showDBs(tokens, Message, dir, entry, root_path);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 1){
            bool calledSuccessfully = createDB(tokens, Message, dir, entry, root_path);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 2){
            bool calledSuccessfully = linkDB(tokens, Message, dir, entry, root_path, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 3){
            bool calledSuccessfully = unlinkDB(tokens, Message, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 4){
            bool calledSuccessfully = delDB(tokens, Message, dir, entry, root_path, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 5){
            bool calledSuccessfully = showTBs(tokens, Message, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 6){
            bool calledSuccessfully = showTBSchema(tokens, Message, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 7){
            bool calledSuccessfully = delTB(tokens, Message, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                return false;
            }
            return true;
        }
        else if(command == 8){
            bool calledSuccessfully = createTB(tokens, Message, cata);
            if (!calledSuccessfully)
            {
                std::cout << "Error found in caller: " << Message << std::endl;
                // if(debugLexer){
                //     std::cout<<"Tokens: "<<std::endl;
                //     Lexer.printTokens(tokens);
                // }
                return false;
            }
            return true;
        }
        return false;
    }
};

#endif
