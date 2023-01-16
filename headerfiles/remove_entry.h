/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __REMOVE_ENTRY_H__
#define __REMOVE_ENTRY_H__
#include <iostream>
#include <string>
#include "count_entries.h"
#include "../fs.h"
#include "../disk.h"
#include <cstring>
int FS::remove_entry(int idx)
{
    // Read current_dir
    uint8_t dir_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, dir_arr);
    struct dir_entry *entry_arr = (struct dir_entry *)dir_arr;

    // Extract size of file
    int size = entry_arr[idx].size;

    // Count number of entries in current_dir
    int no_files = count_entries();

    // Update current_dir entry array
    for (int i = idx; i < no_files; i++)
    {
        entry_arr[i] = entry_arr[i + 1];
    }

    // Write back to disk
    disk.write(current_dir.first_blk, (uint8_t *)entry_arr);

    // Return size
    return size;
}
#endif