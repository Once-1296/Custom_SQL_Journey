#ifndef COMMAND_INTERPRETER_HPP
#define COMMAND_INTERPRETER_HPP

#include "lexer.hpp"

int identifyCommand(std::vector<Token> &tokens, std::string &Message)
{
    if (tokens.empty())
    {
        Message = "No tokens";
        return -1;
    }
    std::variant<std::string, int> firstToken = tokens[0].value;
    if (firstToken.index() != 0)
    {
        Message = "Invalid first token.";
        return -1;
    }
    if (std::get<0>(firstToken) == "SHOW")
    {
        if (tokens.size() < 2)
        {
            Message = "No token after SHOW";
            return -1;
        }
        std::variant<std::string, int> secondToken = tokens[1].value;
        if (secondToken.index() != 0)
        {
            Message = "Invalid second token.";
            return -1;
        }
        if (std::get<0>(secondToken) == "DATABASES")
        {
            return 0;
        }
        else if (std::get<0>(secondToken) == "TABLES")
        {
            return 5;
        }
        else if (std::get<0>(secondToken) == "TABLE")
        {
            return 6;
        }
        Message = "Invalid token after SHOW";    
        return -1;
    }
    else if (std::get<0>(firstToken) == "CREATE")
    {
        if (tokens.size() < 2)
        {
            Message = "No token after CREATE";
            return -1;
        }
        std::variant<std::string, int> secondToken = tokens[1].value;
        if (secondToken.index() != 0)
        {
            Message = "Invalid second token.";
            return -1;
        }
        if(std::get<0>(secondToken) == "DATABASE")
        {
            return 1;
        }
        else if(std::get<0>(secondToken) == "TABLE")
        {
            return 8;
        }
        Message = "Invalid Create command.";
        return -1;
    }
    else if (std::get<0>(firstToken) == "LINK")
    {
        if (tokens.size() < 2)
        {
            Message = "No token after LINK";
            return -1;
        }
        std::variant<std::string, int> secondToken = tokens[1].value;
        if (secondToken.index() != 0)
        {
            Message = "Invalid second token.";
            return -1;
        }
        if (std::get<0>(secondToken) == "DATABASE")
        {
            return 2;
        }
        Message = "Invalid link command";
        return -1;
    }
    else if (std::get<0>(firstToken) == "UNLINK")
    {
        if (tokens.size() < 2)
        {
            Message = "No token after UNLINK";
            return -1;
        }
        std::variant<std::string, int> secondToken = tokens[1].value;
        if (secondToken.index() != 0)
        {
            Message = "Invalid second token.";
            return -1;
        }
        if (std::get<0>(secondToken) == "DATABASE")
        {
            return 3;
        }
        Message = "Invalid unlink command";
        return -1;
    }
    else if (std::get<0>(firstToken) == "DELETE")
    {
        if (tokens.size() < 2)
        {
            Message = "No token after DELETE";
            return -1;
        }
        std::variant<std::string, int> secondToken = tokens[1].value;
        if (secondToken.index() != 0)
        {
            Message = "Invalid second token.";
            return -1;
        }
        if (std::get<0>(secondToken) == "DATABASE")
        {
            return 4;
        }
        else if (std::get<0>(secondToken) == "TABLE")
        {
            return 7;
        }
        return -1;
    }
    else if (std::get<0>(firstToken) == "INSERT")
    {
        return 9;
    }
    return -1;
}

#endif