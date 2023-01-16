/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __COUNT_SLASHES_H__
#define __COUNT_SLASHES_H__
#include <iostream>
#include <string>

int count_slashes(const std::string &s)
{
    // Counter that counts number of slashes
    int count = 0;
    for (char c : s)
    {
        // If c is a slash, increment counter
        if (c == '/')
        {
            count++;
        }
    }
    return count;
}
#endif