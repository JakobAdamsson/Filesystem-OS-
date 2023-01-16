/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __FIND_NAME_IN_DIR_H__
#define __FIND_NAME_IN_DIR_H__
#include <iostream>
#include <string>
#include <cstring>
#include "../fs.h"
#include "../disk.h"
#include "count_entries.h"

int FS::find_name_in_dir(std::string filepath)
{
    // Read current_dir
    uint8_t dir_array[4096];
    disk.read(current_dir.first_blk, dir_array);
    struct dir_entry *entry_arr = (struct dir_entry *)dir_array;

    // Count number of entries in current_dir
    int amount_entries = count_entries();

    // Loop through all entries in current_dir
    for (int i = 0; i < amount_entries; i++)
    {
        // If file_name matches filepath, return index
        if (strcmp(entry_arr[i].file_name, filepath.c_str()) == 0)
        {
            return i;
        }
    }

    // If file_name not found, return -2
    return -2;
}
#endif