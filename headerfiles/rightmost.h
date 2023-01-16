/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __RIGHTMOST_H__
#define __RIGHTMOST_H__

#include <string>

// Returns the rightmost part of a path as a string(std::string)
std::string rightmost(const std::string &str)
{
    // Find the position of the rightmost '/'
    size_t pos = str.rfind('/');
    if (pos == std::string::npos)
    {
        // There is no '/' in the string, rturn the original string
        return str;
    }
    else
    {
        // Return the substring to the right of the rightmost '/'
        return str.substr(pos + 1);
    }
}
#endif