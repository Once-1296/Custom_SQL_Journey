#ifndef COMMAND_INTERPRETER_HPP
#define COMMAND_INTERPRETER_HPP

#include "parse_tree.hpp"

bool identifyCommand(Token &root){
    const Token *firstChild = root.getChildI(0);
    bool error = false;
    std::string errorMessage = "";
    if(firstChild == nullptr){
        errorMessage = "Invalid token at first word.";
        error = true;
        std::cout<<errorMessage<<std::endl;
        return !error;
    }
    if(firstChild -> getValue() == Value("SHOW")){
        const Token *secondChild = root.getChildI(1);
        if(secondChild == nullptr){
            errorMessage = "No token after SHOW";
            error = true;
            std::cout<<errorMessage<<std::endl;
            return !error;
        }
        Value v = secondChild->getValue();
        if(v == Value("DATABASES")){
            return true;
        }
        return false;
    }
    return false;
}

#endif