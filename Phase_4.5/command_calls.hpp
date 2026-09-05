#ifndef COMMAND_CALLER_HPP
#define COMMAND_CALLER_HPP

#include <filesystem>
#include <vector>
#include "Token.hpp"
#include <stdio.h>
#include <string.h>
#include <dirent.h>

bool showDBs(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path)
{
    if (tokens.size() != 2)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    // Open the current directory
    const char *path = file_path.data();
    dir = opendir(path);
    if (dir == NULL)
    {
        Message = "Unable to open directory";
        return false;
    }

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".db" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".db" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            printf("%s\n", entry->d_name);
        }
    }

    // Close the directory
    closedir(dir);
    return true;
}

bool createDB(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path)
{
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (tokens.back().type != tokenType::STR)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::variant<std::string, int> db_token = tokens.back().value;
    if (db_token.index() != 0)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::string db_name = std::get<0>(db_token) + ".db";
    bool exists = false;
    // Open the current directory
    const char *path = file_path.data();
    dir = opendir(path);
    if (dir == NULL)
    {
        Message = "Unable to open directory";
        return false;
    }
    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".db" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".db" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            if (entry->d_name == db_name)
            {
                exists = true;
                break;
            }
        }
    }
    // Close the directory
    closedir(dir);
    if (exists)
    {
        Message = "Database already exists.";
        return false;
    }
    std::string db_path = file_path+"/"+db_name;
    catalog cata(db_path);
    std::cout <<"Succesfully created Database" + db_path<<std::endl;
    return true;
}
bool linkDB(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path, catalog * &cata)
{
    if(cata != nullptr){
        Message = "Already linked to Database "+ cata->getDBName() + " .";
        return false;
    }
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (tokens.back().type != tokenType::STR)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::variant<std::string, int> db_token = tokens.back().value;
    if (db_token.index() != 0)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::string db_name = std::get<0>(db_token) + ".db";
    bool exists = false;
    // Open the current directory
    const char *path = file_path.data();
    dir = opendir(path);
    if (dir == NULL)
    {
        Message = "Unable to open directory";
        return false;
    }
    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".db" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".db" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            if (entry->d_name == db_name)
            {
                exists = true;
                break;
            }
        }
    }
    // Close the directory
    closedir(dir);
    if (!exists)
    {
        Message = "Database does not exist.";
        return false;
    }
    std::string db_path = file_path+"/"+db_name;
    cata = new catalog(db_path);
    std::cout <<"Succesfully linked to Database at" + db_path<<std::endl;
    return true;
}
bool unlinkDB(std::vector<Token> &tokens, std::string &Message, catalog* &cata){
    if(tokens.size() != 2){
        Message = "Invalid tokens at end.";
        return false;
    }
    if(cata == nullptr){
        Message = "No database is linked";
        return false;
    }
    std::string db_name = cata->getDBName();
    delete cata;
    std::cout<<"Successfully unlinked from Database at " + db_name + " ."<<std::endl;
    cata = nullptr;
    return true;
}
bool delDB(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path, catalog* &cata)
{
    if(cata != nullptr){
        Message = "Cannot delete when linked to a Database. Unlink first.";
        return false;
    }
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (tokens.back().type != tokenType::STR)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::variant<std::string, int> db_token = tokens.back().value;
    if (db_token.index() != 0)
    {
        Message = "Invalid DB name";
        return false;
    }
    std::string db_name = std::get<0>(db_token) + ".db";
    bool exists = false;
    // Open the current directory
    const char *path = file_path.data();
    dir = opendir(path);
    if (dir == NULL)
    {
        Message = "Unable to open directory";
        return false;
    }
    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".db" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".db" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            if (entry->d_name == db_name)
            {
                exists = true;
                break;
            }
        }
    }
    // Close the directory
    closedir(dir);
    if (!exists)
    {
        Message = "Database does not exist.";
        return false;
    }
    std::string db_path = file_path+"/"+db_name;
    std::remove(db_path.c_str());
    std::cout <<"Succesfully removed Database" + db_path<<std::endl;
    return true;
}
#endif