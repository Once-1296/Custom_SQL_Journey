#ifndef DB_COMMAND_CALLS_HPP
#define DB_COMMAND_CALLS_HPP

#include <filesystem>
#include <vector>
#include "../types/Token.hpp"
#include "command_validation_helpers.hpp"
#include "../custom_catalog.hpp"
#include <stdio.h>
#include <string.h>

bool showDBs(std::vector<Token> &tokens, std::string &Message, std::filesystem::path &dir, std::string &file_path)
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
    dir = std::filesystem::path{file_path};
    if(!std::filesystem::exists(dir)){
        Message = "Directory doesn't exist";
        return false;
    }
    if(!std::filesystem::is_directory(dir)){
        Message = "Path is not a directory";
        return false;
    }
    for(auto& dirent : std::filesystem::directory_iterator{dir}){
        const std::string itemPath = dirent.path().string();
        if(itemPath.size() < 4){
            continue;
        }
        std::string ext = itemPath.substr(itemPath.size()-3,3);
        if(ext == ".db"){
            std::cout << itemPath << std::endl;
        }
    }
    return true;
}

bool createDB(std::vector<Token> &tokens, std::string &Message, std::filesystem::path dir, std::string &file_path)
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
    auto chk = fileExists(dir, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (exists)
    {
        Message += "\nDatabase already exists.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    // std::cout<<"Path :  "<<db_path<<std::endl;
    catalog cata(db_path);
    std::cout << "Succesfully created Database " + db_path << std::endl;
    return true;
}
bool linkDB(std::vector<Token> &tokens, std::string &Message, std::filesystem::path dir, std::string &file_path, catalog *&cata)
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
    auto chk = fileExists(dir, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (!exists)
    {
        Message += "\nDatabase does not exist.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    // std::cout<<"Path :  "<<db_path<<std::endl;
    cata = new catalog(db_path);
    std::cout << "Succesfully linked to Database at " + db_path << std::endl;
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
bool delDB(std::vector<Token> &tokens, std::string &Message, std::filesystem::path dir, std::string &file_path, catalog *&cata)
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
    auto chk = fileExists(dir, file_path, db_name,Message);
    bool success = chk.first;
    bool exists = chk.second;
    if(!success)return false;
    if (!exists)
    {
        Message += "\nDatabase does not exist.";
        return false;
    }
    std::string db_path = file_path + "/" + db_name;
    std::remove(db_path.c_str());
    std::cout << "Succesfully removed Database " + db_path << std::endl;
    return true;
}
#endif