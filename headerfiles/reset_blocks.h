/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __RESET_BLOCKS_H__
#define __RESET_BLOCKS_H__

#include <iostream>
#include <string>
#include <math.h>
#include "../fs.h"
#include "../disk.h"
#include <cstring>
void FS::reset_blocks(int idx, int size)
{
    // Read FAT
    int16_t fat_array[BLOCK_SIZE / 2];
    disk.read(1, (uint8_t *)fat_array);

    // Count number of blocks
    int no_blocks = ceil((float)(size) / (float)BLOCK_SIZE);

    // Make int variable we use for next index in the for loop
    int next_id;
    for (int i = 0; i < no_blocks; i++)
    {
        // Extract next index before we reset current index
        int next_id = fat_array[idx];
        fat_array[idx] = 0;
        idx = next_id;
    }
    // Write FAT back to disk
    disk.write(1, (uint8_t *)fat_array);
}
#endif