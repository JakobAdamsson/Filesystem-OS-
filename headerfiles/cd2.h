/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __CD2_H__
#define __CD2_H__

#include <iostream>
#include <string>
#include "getblock.h"
#include "../fs.h"
#include "../disk.h"
#include <cstring>
#include "count_slashes.h"
#include "remove_prefix.h"
#include "errors.h"
#include "find_name_in_dir.h"

int FS::cd2(std::string dirpath)
{
    // Save current directory
    struct dir_entry restore = current_dir;

    // Check if we want to cd to root
    if (dirpath == "/")
    {
        current_dir = root_dir;
        return 0;
    }

    // Check if we are using absolute path
    if (dirpath[0] == '/')
    {
        // If absolute, set current_dir to root and convert to relative path
        current_dir = root_dir;
        dirpath.erase(0, 1);
    }

    // Count the number of slashes in the path
    int slashes = count_slashes(dirpath);

    // Number of directories to cd into
    int number_of_directiores = slashes;

    // Loop through the directories
    for (int i = 0; i < number_of_directiores; i++)
    {
        // Get the name of the directory
        std::string output = remove_prefix(dirpath);

        // Check if the directory exists
        int idx = find_name_in_dir(output);
        if (idx == -2)
        {
            printf(CD_ERROR_NOT_FOUND);
            current_dir = restore;
            return -2;
        }

        // Read current directory
        uint8_t data[BLOCK_SIZE];
        disk.read(current_dir.first_blk, data);
        struct dir_entry *entry_arr = (dir_entry *)data;

        // Update current_dir
        current_dir = entry_arr[idx];
    }

    return 0;
}
#endif