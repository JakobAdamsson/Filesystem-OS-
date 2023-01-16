/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __REMOVE_PREFIX_H__
#define __REMOVE_PREFIX_H__

#include <string>
#include <iostream>
#include <vector>

std::string remove_prefix(std::string &s)
{
    // Find index of first slash
    size_t pos = s.find("/");
    // If no slash exist return
    if (pos == std::string::npos)
    {
        std::string result = s;

        // Clears string
        s.clear();
        return result;
    }
    else
    {
        // Extract everything before first slash
        std::string result = s.substr(0, pos);
        // Clear everything before first slash
        s.erase(0, pos + 1); // ta bort prefixet inplace

        // Return prefix
        return result;
    }
}
#endif