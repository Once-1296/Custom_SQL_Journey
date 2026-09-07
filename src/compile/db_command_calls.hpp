#ifndef DB_COMMAND_CALLS_HPP
#define DB_COMMAND_CALLS_HPP

#include <filesystem>
#include <vector>
#include "../types/Token.hpp"
#include "command_validation_helpers.hpp"
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
    auto check_SHOW = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "SHOW");
    if (!check_SHOW.first)
    {
        Message += "\n : Invalid token instead of SHOW keyword";
        return false;
    }
    auto check_DBS = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "DATABASES");
    if (!check_DBS.first)
    {
        Message += " \n : Invalid token instead of DATABASES keyword";
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
    auto check_CREATE = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "CREATE");
    if (!check_CREATE.first)
    {
        Message += "\n : Invalid token instead of CREATE keyword";
        return false;
    }
    auto check_DB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "DATABASE");
    if (!check_DB.first)
    {
        Message += "\n : Invalid token instead of DATABASE keyword";
        return false;
    }
    auto check_DBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_DBName.first)
    {
        Message += "\n : Invalid token for DB Name.";
        return false;
    }
    auto db_token = check_DBName.second;
    std::string db_name = std::get<0>(db_token) + ".db";
    auto chk = fileExists(dir, entry, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (exists)
    {
        Message = "Database already exists.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    catalog cata(db_path);
    std::cout << "Succesfully created Database" + db_path << std::endl;
    return true;
}
bool linkDB(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path, catalog *&cata)
{
    if (cata != nullptr)
    {
        Message = "Already linked to Database " + cata->getDBName() + " .";
        return false;
    }
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    auto check_LINK = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "LINK");
    if (!check_LINK.first)
    {
        Message += "\n : Invalid token instead of LINK keyword";
        return false;
    }
    auto check_DB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "DATABASE");
    if (!check_DB.first)
    {
        Message += "\n : Invalid token instead of DATABASE keyword";
        return false;
    }
    auto check_DBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_DBName.first)
    {
        Message += "\n : Invalid token for DB Name.";
        return false;
    }
    auto db_token = check_DBName.second;
    std::string db_name = std::get<0>(db_token) + ".db";
    // std::cout<<"Name :  "<<db_name<<std::endl;
    auto chk = fileExists(dir, entry, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (!exists)
    {
        Message = "Database does not exist.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    // std::cout<<"Path :  "<<db_path<<std::endl;
    cata = new catalog(db_path);
    std::cout << "Succesfully linked to Database at" + db_path << std::endl;
    return true;
}
bool unlinkDB(std::vector<Token> &tokens, std::string &Message, catalog *&cata)
{
    if (tokens.size() != 2)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    if (cata == nullptr)
    {
        Message = "No database is linked";
        return false;
    }
    auto check_UNLINK = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "UNLINK");
    if (!check_UNLINK.first)
    {
        Message += "\n : Invalid token instead of UNLINK keyword";
        return false;
    }
    auto check_DB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "DATABASE");
    if (!check_DB.first)
    {
        Message += "\n : Invalid token instead of DATABASE keyword";
        return false;
    }
    std::string db_name = cata->getDBName();
    delete cata;
    std::cout << "Successfully unlinked from Database at " + db_name + " ." << std::endl;
    cata = nullptr;
    return true;
}
bool delDB(std::vector<Token> &tokens, std::string &Message, DIR *dir, struct dirent *entry, std::string &file_path, catalog *&cata)
{
    if (cata != nullptr)
    {
        Message = "Cannot delete when linked to a Database. Unlink first.";
        return false;
    }
    if (tokens.size() != 3)
    {
        Message = "Invalid tokens at end.";
        return false;
    }
    auto check_DEL = validTokenAt(0, tokens, Message, {tokenType::KEYWORD}, 0, true, "DELETE");
    if (!check_DEL.first)
    {
        Message += "\n : Invalid token instead of DELETE keyword";
        return false;
    }
    auto check_DB = validTokenAt(1, tokens, Message, {tokenType::KEYWORD}, 0, true, "DATABASE");
    if (!check_DB.first)
    {
        Message += "\n : Invalid token instead of DATABASE keyword";
        return false;
    }
    auto check_DBName = validTokenAt(2, tokens, Message, {tokenType::STR, tokenType::FORCE_STR}, 0);
    if (!check_DBName.first)
    {
        Message += "\n : Invalid token for DB Name.";   
        return false;
    }
    auto db_token = check_DBName.second;
    std::string db_name = std::get<0>(db_token) + ".db";
    auto chk = fileExists(dir, entry, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (!exists)
    {
        Message = "Database does not exist.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    std::remove(db_path.c_str());
    std::cout << "Succesfully removed Database" + db_path << std::endl;
    return true;
}
#endif
