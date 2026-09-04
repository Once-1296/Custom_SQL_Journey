#ifndef COMMAND_CALLER_HPP
#define COMMAND_CALLER_HPP

#include <filesystem>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

void showDBs(DIR *dir, struct dirent* entry, std::string &file_path)
{
    // Open the current directory
    const char* path = file_path.data();
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("Unable to open directory");
        return;
    }

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Find the location of ".txt" in the filename
        char *ext = strstr(entry->d_name, ".db");

        // Ensure ".txt" exists and is at the very end of the filename
        if (ext != NULL && strcmp(ext, ".db") == 0)
        {
            printf("%s\n", entry->d_name);
        }
    }

    // Close the directory
    closedir(dir);
}
#endif