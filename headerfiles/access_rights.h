/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __ACCESS_RIGHTS_H__
#define __ACCESS_RIGHTS_H__
#include <string.h>
#include <string>
#include "../fs.h"

std::string
access_rights_str(struct dir_entry x)
{
    // Set the access rights string
    std::string str;

    // Check if the access rights are set to read
    int bit = (x.access_rights >> 2) & 1;
    if (bit == 1)
    {
        str = str + "r";
    }

    // If not set to read, add a '-'
    else
    {
        str = str + "-";
    }

    // Check if the access rights are set to write
    bit = (x.access_rights >> 1) & 1;
    if (bit == 1)
    {
        str = str + "w";
    }

    // If not set to write, add a '-'
    else
    {
        str = str + "-";
    }

    // Check if the access rights are set to execute
    bit = (x.access_rights) & 1;
    if (bit == 1)
    {
        str = str + "x";
    }

    // If not set to execute, add a '-'
    else
    {

        str = str + "-";
    }

    // Return the access rights string
    return str;
}
#endif