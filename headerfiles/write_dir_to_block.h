/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __WRITE_DIR_TO_FILE_H__
#define __WRITE_DIR_TO_FILE_H__

#include <iostream>
#include <string>
#include "getblock.h"
#include "../fs.h"
#include "../disk.h"
#include <cstring>

int FS::write_dir_to_block(struct dir_entry file)
{
    // Read FAT
    int16_t fat_array[BLOCK_SIZE / 2];
    disk.read(1, (uint8_t *)fat_array);

    // Get index of the next free block
    int free_idx = free_block(fat_array);

    uint8_t dir_arr[BLOCK_SIZE];
    disk.write(free_idx, dir_arr);

    // Set index to taken
    fat_array[free_idx] = -1;

    // Write back updated FAT
    disk.write(1, (uint8_t *)fat_array);
    return free_idx;
};

#endif