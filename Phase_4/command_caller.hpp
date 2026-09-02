#ifndef COMMAND_CALLER_HPP
#define COMMAND_CALLER_HPP

#include <filesystem>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
DIR *dir;
struct dirent *entry;
class Caller
{
public:
    void showDBs()
    {
        // Open the current directory
        dir = opendir(".");
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
};
#endif