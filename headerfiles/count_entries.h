/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __COUNT_ENTRIES_H__
#define __COUNT_ENTRIES_H__
#include <iostream>
#include <string>
#include <cstring>
#include "../fs.h"
#include "../disk.h"
int FS::count_entries()
{
    // Read current_dir
    uint8_t temp_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, temp_arr);
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;

    // Counter that counts number of entries
    int counter = 0;
    for (int i = 0; i < 64; i++)
    {

        // If file_name is not empty, increment counter(since its initially set to "" in fs.h)
        if ((strcmp(entry_arr[i].file_name, "")))
        {
            counter++;
        }
        else
        {
            return counter;
        }
    }
    return counter;
}
#endif