/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __READ_FILE_CONTENT_H__
#define __READ_FILE_CONTENT_H__
#include <iostream>
#include <string>
#include <math.h>
#include "../fs.h"
#include "../disk.h"
#include <cstring>

// Returns a charpoint with content of file of dir_entry argument. Assumes caller uses free()
char *FS::read_file_content(struct dir_entry file)
{
    // Read FAT
    int16_t fat_array[BLOCK_SIZE / 2];
    disk.read(1, (uint8_t *)fat_array);

    // Count number of blocks needed to be read from
    int no_blocks = ceil((float)(file.size) / (float)BLOCK_SIZE);

    // Allocate memory to store content of file
    char *ptr = (char *)malloc(no_blocks * BLOCK_SIZE);

    // Create array with size of block for temporary use
    uint8_t temp_arr[BLOCK_SIZE];

    // Keep track of previous block to get index of next block
    int prev_block = file.first_blk;
    for (int i = 0; i < no_blocks; i++)
    {
        // Read blcok
        disk.read(prev_block, temp_arr);
        // Store content in our allocated memory at apppropiate index
        memcpy(ptr + i * BLOCK_SIZE, temp_arr, BLOCK_SIZE);

        // Get next index
        prev_block = fat_array[prev_block];
    }
    // Return pointer
    return ptr;
}
#endif