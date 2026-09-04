#ifndef COMMAND_INTERPRETER_HPP
#define COMMAND_INTERPRETER_HPP

#include "parse_tree.hpp"

int identifyCommand(Token &root, std::string &Message){
    const Token *firstChild = root.getChildI(0);
    
    if(firstChild == nullptr){
        Message = "Invalid token at first word.";
        return -1;
    }
    if(firstChild -> getValue() == Value("SHOW")){
        const Token *secondChild = root.getChildI(1);
        if(secondChild == nullptr){
            Message = "No token after SHOW";
            return -1;
        }
        Value v = secondChild->getValue();
        if(v == Value("DATABASES")){
            return 0;
        }
        return -1;
    }
    return -1;
}

#endif