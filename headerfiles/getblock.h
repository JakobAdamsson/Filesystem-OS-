/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __GETBLOCK_H__
#define __GETBLOCK_H__
#include <cstring>
#include <cstdint>

#define BLOCK_SIZE 4096
#define FAT_FREE 0
int free_block(int16_t *fat)
{
    // Iterate through FAT and return first free block
    for (int i = 0; i < BLOCK_SIZE / 2; i++)
    {
        // If block is free, return index
        if (fat[i] == FAT_FREE)
        {
            return i;
        }
    }
    return -2;
}
#endif