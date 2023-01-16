/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __ADD_ENTRY_H__
#define __ADD_ENTRY_H_
#include <iostream>
#include <string>
#include <cstring>
#include "../fs.h"
#include "../disk.h"
#include "count_entries.h"

int FS::add_entry(struct dir_entry new_file)
{
    // Check if there is space in the current directory
    int idx = count_entries();

    // Read current directory
    uint8_t temp_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, temp_arr);
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;

    // Add new entry to current directory
    entry_arr[idx] = new_file;

    // Write back to disk
    disk.write(current_dir.first_blk, (uint8_t *)entry_arr);
    return 0;
}
#endif