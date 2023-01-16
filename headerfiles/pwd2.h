/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __PWD2_H__
#define __PWD2_H__
#include <vector>
#include <iostream>
#include "../fs.h"
#include <fstream>
#include <cstring>

std::string FS::pwd2()
{

    // Check if the current directory is the root directory
    if (current_dir.first_blk == 0)
    {
        std::string start = "/";
        return start;
    }

    // Create vector to store the first_black in each directory, will be used as a stack later
    std::vector<int> myVector;

    // Check if we are at the root
    while (current_dir.first_blk)
    {
        // push first block
        myVector.push_back(current_dir.first_blk);

        // change current dir to parent
        cd("..");
    }

    // Set current_dir to root_dir since we don't want current_dir.filename to be ".."
    current_dir = root_dir;

    // Read current_dir
    uint8_t dir_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, dir_arr);
    struct dir_entry *entry_arr = (dir_entry *)dir_arr;

    // Counts how many first blocks we need to read
    int vector_size = myVector.size();
    // Create a vector to store the names of the directories in the path
    std::vector<std::string> path;
    // Traverse the directories in the path and store their names in the vector
    for (int i = 0; i < vector_size; i++)
    {
        // Count number of files in current_dir
        int no_files = count_entries();
        // find index
        int block_to_find = myVector[vector_size - i - 1];
        myVector.pop_back();
        for (int k = 0; k < no_files; k++)
        {
            if (entry_arr[k].type == 1)
            {
                if (block_to_find == entry_arr[k].first_blk)
                {
                    path.push_back("/");
                    path.push_back(entry_arr[k].file_name);
                    current_dir = entry_arr[k];
                    disk.read(entry_arr[k].first_blk, (uint8_t *)entry_arr);
                }
            }
        }
    }

    // Create a string to store the path
    std::string result;

    // Traverse the vector and add the names of the directories to the string
    for (int i = 0; i < path.size(); i++)
    {
        result += path[i];
    }

    // Return the vector of directory names
    return result;
}
#endif