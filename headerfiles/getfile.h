/*
By Jakob Adamsson and Emil Gullbrandsson
*/

#include <fstream>
#ifndef __GETFILE_H__
#define __GETFILE_H__
#include <iostream>
#include <cstring>

// Contains information about the newly created file
typedef struct filedata
{
    char *user_input;
    int size;

} filedata;

// Uses malloc, assumes that the caller frees the pointer.
// Returns instance of struct(filedata)
char *get_file(void)
{
    // Allocate memory for filedata struct
    filedata new_file;

    // Allocate memory for user input
    char *temp_arr = (char *)malloc(1);

    // Pointer to new memory location
    void *newpointer;
    new_file.size = 0;

    // Get user input, loop until user enters empty line
    while (true)
    {
        // Get user input from stdin and store in temp
        std::string temp;
        getline(std::cin, temp);

        // Get lenght of user input
        int n = temp.length();

        // Add one to get \0(Null terminator)
        char arr[n + 1];

        // If user enters empty line, break loop
        if (temp.empty())
        {

            temp_arr[new_file.size - 1] = '\0';
            break;
        }

        // If the user enters a line with content
        else
        {
            // No need for +1 due to we allocated n + 1 on line 44(arr[n+1])
            strncpy(arr, temp.c_str(), n);
            strncpy(arr + n, "\n", 1);

            // Reallocate memory for temp_arr(So that it fits the new input)
            if (newpointer = realloc(temp_arr, new_file.size + n + 1))
            {
                // If reallocation was successful, copy content of arr to temp_arr
                temp_arr = (char *)newpointer;
                memcpy(temp_arr + new_file.size, arr, n + 1);
            }
            // Update size of temp_arr(+1 for newline)
            new_file.size += n + 1;
        }
    }

    // Return pointer to temp_arr(needs to be freed by caller)
    return temp_arr;
}

#endif