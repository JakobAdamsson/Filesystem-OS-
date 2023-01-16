/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __WRITE_BLOCKS_H__
#define __WRITE_BLOCKS_H__

#include <iostream>
#include <string>
#include <cstring>
#include "../fs.h"
#include "../disk.h"
#include <math.h>
#include "getblock.h"

int FS::write_blocks(char *content)
{
    // Read FAT
    int16_t fat_array[BLOCK_SIZE / 2];
    disk.read(1, (uint8_t *)fat_array);

    // Count how many blocks we need to write to
    int no_blocks = ceil((float)(strlen(content) + 1) / (float)BLOCK_SIZE);

    // Create temporary array with the size of a block that we can use to write to disk with
    uint8_t temp_arr[BLOCK_SIZE];
    // Get index of free block for start
    int free_idx = free_block(fat_array);
    int start_blk = free_idx;
    for (int i = 0; i < no_blocks; i++)
    {
        // Move 4096 bytes to tempt_ for content to write to disk
        strncpy((char *)temp_arr, content + i * BLOCK_SIZE, BLOCK_SIZE);
        // WRite chunk to disk
        disk.write(free_idx, temp_arr);
        // Update FAT
        fat_array[free_idx] = free_block(fat_array);

        if (i < no_blocks - 1)
        {
            free_idx = fat_array[free_idx];
        }
    }
    fat_array[free_idx] = -1;
    // Write fat to disk
    disk.write(1, (uint8_t *)fat_array);

    // Returns starting block of file
    return start_blk;
}

#endif