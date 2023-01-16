
/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __FIND_SUBFOLDER_H__
#define __FIND_SUBFOLDER_H__
#include <iostream>
#include <string>

// std::string to filepath, const std::string to what we want to find
// Returns 0 on success, 1 otherwise
int find_subfolder(std::string &path, const std::string &prefix)
{
    // If path starts with a slash, remove it so that we get a relative path
    if (path[0] == '/')
    {
        path.erase(0, 1);
    }

    // Find the position of the prefix in the path
    size_t prefixPos = path.find(prefix);

    // If the prefix was found at the beginning of path
    if (prefixPos == 0)
    {
        // remove the prefix from path
        path.erase(0, prefix.size());
        return 0;
    }

    return -2;
}
#endif