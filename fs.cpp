/*

By Jakob Adamsson And Emil Gullbrandsson
DVAMI20h

*/
#include <iostream>
#include "fs.h"
#include <fstream>
#include <cstring>
#include <math.h>
#include <typeinfo>
#include <type_traits>
#include <time.h>
#include <chrono>
#include <vector>

// headerfiles
#include "headerfiles/access_rights.h"
#include "headerfiles/add_entry.h"
#include "headerfiles/cd2.h"
#include "headerfiles/count_entries.h"
#include "headerfiles/count_slashes.h"
#include "headerfiles/errors.h"
#include "headerfiles/find_name_in_dir.h"
#include "headerfiles/find_subfolder.h"
#include "headerfiles/getblock.h"
#include "headerfiles/getfile.h"
#include "headerfiles/pwd2.h"
#include "headerfiles/read_file_content.h"
#include "headerfiles/remove_entry.h"
#include "headerfiles/remove_prefix.h"
#include "headerfiles/reset_blocks.h"
#include "headerfiles/rightmost.h"
#include "headerfiles/write_blocks.h"
#include "headerfiles/write_dir_to_block.h"

FS::FS()
{
    std::cout << "FS::FS()... Creating file system\n";
}

FS::~FS()
{
}

// formats the disk, i.e., creates an empty file system
int FS::format()
{
    using namespace std;
    // Create the fat array
    int16_t fat_array[BLOCK_SIZE / 2];
    disk.read(1, (uint8_t *)fat_array);
    fat_array[0] = FAT_EOF;
    fat_array[1] = FAT_EOF;
    strncpy(root_dir.file_name, "/", 2);
    root_dir.size = 0;
    root_dir.first_blk = 0;
    root_dir.type = 1;
    root_dir.access_rights = 0x04;
    for (int16_t i = 2; i < BLOCK_SIZE / 2; i++)
    {
        fat_array[i] = FAT_FREE;
    }
    // Initialize the root directory
    dir_entry empty_arr[64];

    // Write the fat array and the root directory to the disk
    disk.write(0, (uint8_t *)empty_arr);
    disk.write(1, (uint8_t *)fat_array);

    // Set the current directory to the root directory
    current_dir = root_dir;
    return 0;
}

// create <filepath> creates a new file on the disk, the data content is
// written on the following rows (ended with an empty row)
int FS::create(std::string filepath)
{

    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Extract the filename from the filepath
    std::string dirpath_copy = rightmost(filepath);

    // Change the current directory to the directory of the file
    cd2(filepath);

    // Check if filename is too long
    if (filepath.length() > 55)
    {

        printf(NAME_TO_LONG_ERROR);
        cd(restore_point);
        return -2;
    }

    // Check if the current directory is full
    int no_files_in_dir = count_entries();
    if (no_files_in_dir > 63)
    {
        printf(CREATE_ERROR);
        cd(restore_point);
        return -2;
    }

    // Get the content of the file
    char *content = get_file();

    // Write the content to the disk
    int idx = write_blocks(content);

    // Initialize the new file
    struct dir_entry new_file;

    // Set its properties
    strncpy(new_file.file_name, dirpath_copy.c_str(), filepath.length() + 1);
    new_file.size = strlen(content) + 1;
    new_file.first_blk = idx;
    new_file.type = 0;
    new_file.access_rights = READ | WRITE;

    // Write the new file to the current directory
    add_entry(new_file);

    // Free the memory
    free(content);

    // Change the current directory back to the original(restor point)
    cd(restore_point);
    return 0;
}

// cat <filepath> reads the content of a file and prints it on the screen
int FS::cat(std::string filepath)
{
    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Extract the filename from the filepath
    std::string filename = rightmost(filepath);

    // Change the current directory to the directory of the file
    cd2(filepath);

    // Check if the file exists in the current directory
    int idx = find_name_in_dir(filename);
    if (idx == -2)
    {
        printf(CAT_ERROR);
        cd(restore_point);
        return -2;
    }

    // Read the current directory
    uint8_t temp_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, temp_arr);
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;

    // Check if the entry is a file
    if (entry_arr[idx].type == 1)
    {
        printf(NOT_A_FILE);
        cd(restore_point);
        return -2;
    }

    // Check if the user has read access, if not return error and change the current directory back to the original(restore point)
    int bit = (entry_arr[idx].access_rights >> 2) & 1;
    if (bit == 1)
    {
        char *ptr = read_file_content(entry_arr[idx]);
        printf("%s\n", ptr);
    }
    else
    {
        printf(NO_READ_ACCESS);
        cd(restore_point);
        return -2;
    }
    // Change the current directory back to the original(restore point)
    cd(restore_point);
    return 0;
}

// ls lists the content in the currect directory (files and sub-directories)
int FS::ls()
{
    // Read the current_dir block to get all entries
    uint8_t root_arr[4096];
    disk.read(current_dir.first_blk, root_arr);
    struct dir_entry *entry_arr = (struct dir_entry *)root_arr;

    // Count the number of entries in the current directory
    int no_entries = count_entries();
    printf("Name\t type\taccessrights\tSize \n");

    // Display the directories
    for (int i = 0; i < no_entries; i++)
    {
        if (entry_arr[i].type == 1 && strcmp(entry_arr[i].file_name, "..") != 0)
        {
            printf("%s\t dir\t %s\t -\n", entry_arr[i].file_name, access_rights_str(entry_arr[i]).c_str());
        }
    }

    // Display the files
    for (int i = 0; i < no_entries; i++)
    {
        if (entry_arr[i].type == 0)
        {
            printf("%s\t file\t %s\t %d\n", entry_arr[i].file_name, access_rights_str(entry_arr[i]).c_str(), entry_arr[i].size);
        }
    }
    return 0;
}

// cp <sourcepath> <destpath> makes an exact copy of the file
// <sourcepath> to a new file <destpath>
int FS::cp(std::string sourcepath, std::string destpath)
{
    // Extract the filename from the sourcepath
    std::string src_name = rightmost(sourcepath);

    // Extract the filename from the destpath
    std::string dest_name = rightmost(destpath);

    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Change the current directory to the directory of the source file
    cd2(sourcepath);

    // Check if the source file exists in the current directory
    int src_idx = find_name_in_dir(src_name);
    if (src_idx == -2)
    {
        printf(CP_ERROR_SRC);
        cd(restore_point);
        return -2;
    }

    // Read the current directory
    uint8_t temp_arr[BLOCK_SIZE];
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);

    // Make a copy of the source file entry so that we can change the name
    struct dir_entry dir_entry_copy = entry_arr[src_idx];

    // Read the content of the source file
    char *content = read_file_content(entry_arr[src_idx]);

    // Change the current directory to the directory of the destination file
    cd2(destpath);

    // Change name of the source file entry to the destination file name
    strncpy(dir_entry_copy.file_name, dest_name.c_str(), src_name.length() + 1);
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);

    // Check if the destination file exists in the current directory
    int dest_idx = find_name_in_dir(dest_name);
    if (dest_idx != -2)
    {
        if (entry_arr[dest_idx].type == 0)
        {
            printf(CP_ERROR_DEST);
            free(content);
            cd(restore_point);
            return -2;
        }
        else
        {
            cd(dest_name);
            strncpy(dir_entry_copy.file_name, src_name.c_str(), src_name.length() + 1);
        }
    }

    // Write the content of the source file to the destination file
    dir_entry_copy.first_blk = write_blocks(content);

    // Write the destination file entry to the current directory
    add_entry(dir_entry_copy);

    // Free the memory allocated for the content of the source file
    free(content);

    // Go back to the original directory(Restore point)
    cd(restore_point);
    return 0;
}

// mv <sourcepath> <destpath> renames the file <sourcepath> to the name <destpath>,
// or moves the file <sourcepath> to the directory <destpath> (if dest is a directory)

int FS::mv(std::string sourcepath, std::string destpath)
{
    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Extract the filename from the sourcepath
    std::string src_name = rightmost(sourcepath);

    // Extract the filename from the destpath
    std::string dest_name = rightmost(destpath);

    // Change current directory to sourcefile and handle the name
    cd2(sourcepath);

    // Check if the source file exists in the current directory
    int src_idx = find_name_in_dir(src_name);
    if (src_idx == -2)
    {
        printf(SRC_NOT_FOUND_ERROR);
        cd(restore_point);
        return -2;
    }

    // Read the current directory
    uint8_t temp_arr[BLOCK_SIZE];
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);

    // New entry dir
    struct dir_entry dir_entry_copy = entry_arr[src_idx];
    strncpy(dir_entry_copy.file_name, dest_name.c_str(), dest_name.length() + 1);

    // Save point we can go back to remove entry, we don't do it now incase of a later error
    std::string remove_point = pwd2();

    // Change current directory to destfile
    cd2(destpath);

    // Check if the destination file exists in the current directory
    int dest_idx = find_name_in_dir(dest_name);
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);
    if (dest_idx != -2)
    {
        if (entry_arr[dest_idx].type == 0)
        {
            printf(DEST_EXISTING);
            cd(restore_point);
            return -2;
        }
        else
        {
            // Change current directory to the destination directory
            cd(dest_name);

            // Copy the source file name to the destination file name
            strncpy(dir_entry_copy.file_name, src_name.c_str(), src_name.length() + 1);
        }
    }

    // Write the destination file entry to the current directory
    add_entry(dir_entry_copy);

    // Change current directory to the directory of the source file, where we removed the entry
    cd(remove_point);

    // Remove the source file entry from the current directory
    remove_entry(src_idx);

    // Go back to the original directory(Restore point)
    cd(restore_point);
    return 0;
}

// rm <filepath> removes / deletes the file <filepath>
int FS::rm(std::string filepath)
{
    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Extract the filename from the filepath
    std::string file_name = rightmost(filepath);

    // Change current directory to the directory of the file
    cd2(filepath);

    // Check if the file exists in the current directory
    int idx = find_name_in_dir(file_name);
    if (idx == -2)
    {
        printf(RM_ERROR);
        cd(restore_point);
        return -2;
    }

    // Read the current directory
    uint8_t dir_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, dir_arr);
    struct dir_entry *entry_arr = (dir_entry *)dir_arr;

    // Remove the file entry from the current directory
    int size = remove_entry(idx);

    // Free the blocks used by the file
    reset_blocks(entry_arr[idx].first_blk, size);

    // Go back to the original directory(Restore point)
    cd(restore_point);
    return 0;
}

// append <filepath1> <filepath2> appends the contents of file <filepath1> to
// the end of file <filepath2>. The file <filepath1> is unchanged.
int FS::append(std::string filepath1, std::string filepath2)
{
    // Save the current directory filepath
    std::string restore_point = pwd2();

    // Extract the filename from the filepath1
    std::string file_name1 = rightmost(filepath1);

    // Extract the filename from the filepath2
    std::string file_name2 = rightmost(filepath2);

    // Change current dir to filpath1
    cd2(filepath1);

    // Check if the file exists in the current directory
    int idx1 = find_name_in_dir(file_name1);
    if (idx1 == -2)
    {
        printf(FILEPATH1_NOT_FOUND);
        return -2;
    }

    // Read the current directory
    uint8_t dir_arr[BLOCK_SIZE];
    disk.read(current_dir.first_blk, dir_arr);
    struct dir_entry *entry_arr = (dir_entry *)dir_arr;

    // Check read permision on f1
    int bit1 = (entry_arr[idx1].access_rights >> 2) & 1;
    if (bit1 != 1)
    {
        printf(NO_READ_ACCESS_FILEPATH1);
        cd(restore_point);
        return 0;
    }

    // Read the file content of f1(needs to be freed at some point)
    char *content1 = read_file_content(entry_arr[idx1]);

    // Change current dir to filepath2
    cd2(filepath2);

    // See if file exists in current dir
    int idx2 = find_name_in_dir(file_name2);
    if (idx2 == -2)
    {
        free(content1);
        printf(FILEPATH2_NOT_FOUND);
        cd(restore_point);
        return -2;
    }

    // Read the current directory
    disk.read(current_dir.first_blk, dir_arr);
    entry_arr = (dir_entry *)dir_arr;

    // check write permision on f2
    int bit2 = (entry_arr[idx2].access_rights >> 1) & 1;
    if (bit2 != 1)
    {
        printf(NO_WRITE_ACCESS_FILEPATH2);
        free(content1);
        cd(restore_point);
        return -2;
    }

    // Read the file content of f2(needs to be freed at some point)
    char *content2 = read_file_content(entry_arr[idx2]);

    // Concatenate the contents of f1 and f2
    char concat_arr[strlen(content1) + strlen(content2) + 1];

    // Copy the contents of f2 to the concat_arr
    memcpy(concat_arr, content2, strlen(content2));

    memcpy(concat_arr + strlen(content2), "\n", strlen("\n"));
    // Copy the contents of f1 to the concat_arr
    memcpy(concat_arr + 1 + strlen(content2), content1, strlen(content1) + 1);

    // Read the current directory
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);

    // Free the blocks used by f2
    reset_blocks(entry_arr[idx2].first_blk, entry_arr[idx2].size);

    // Write the new content to the blocks
    entry_arr[idx2].first_blk = write_blocks(concat_arr);
    entry_arr[idx2].size = strlen(concat_arr) + 1;

    // Write the current directory to disk
    disk.write(current_dir.first_blk, (uint8_t *)entry_arr);

    // Free the memory allocated for the contents
    free(content1);
    free(content2);

    // Go back to the original directory(Restore point)
    cd(restore_point);
    return 0;
}

// mkdir <dirpath> creates a new sub-directory with the name <dirpath>
// in the current directory
int FS::mkdir(std::string dirpath)
{
    // Create a restore point
    std::string restore_point = pwd2();
    // Save a copy of the path
    std::string dirpath_copy = dirpath;

    // Set current_dir to where new directory should be placed
    cd2(dirpath);

    // Check so that a file the name doesn't exist
    if (find_name_in_dir(dirpath) != -2)
    {
        printf(FILE_EXISTING_DIR);
        cd(restore_point);
        return -2;
    }

    // Extract name of the new directory
    dirpath_copy = rightmost(dirpath_copy);

    // Create entry_dir for the new directory
    struct dir_entry new_dir;
    strncpy(new_dir.file_name, dirpath_copy.c_str(), dirpath_copy.length() + 1);
    new_dir.type = 1;
    new_dir.first_blk = write_dir_to_block(new_dir);
    new_dir.access_rights = READ | WRITE | EXECUTE;

    // Add new entry_dir to current_dir
    add_entry(new_dir);

    // save first block of parent for when we add an entry for parent in new directory
    int parent_first_blk = current_dir.first_blk;

    // MOVE IN TO THE NEW DIR
    current_dir = new_dir;

    // Write a clean dir_entry array to disk
    struct dir_entry reset_arr[64];
    disk.write(current_dir.first_blk, (uint8_t *)reset_arr);

    // create a new entry with name ".." for parent folder.
    struct dir_entry parent_folder;
    strncpy(parent_folder.file_name, "..", 3);
    parent_folder.first_blk = parent_first_blk;
    parent_folder.type = 1;
    add_entry(parent_folder);

    // Cd back to where we started
    cd(restore_point);
    return 0;
}

// cd <dirpath> changes the current (working) directory to the directory named <dirpath>
int FS::cd(std::string dirpath)
{
    // Save restore point incase of error
    struct dir_entry restore = current_dir;
    // Check if we want to cd to root
    if (dirpath == "/")
    {
        current_dir = root_dir;
        return 0;
    }

    // Check if we are using absolute path
    if (dirpath[0] == '/')
    {
        // If absolute, set current_dir to root and convert to relative path
        current_dir = root_dir;
        dirpath.erase(0, 1);
    }

    // Count number of slashes
    int slashes = count_slashes(dirpath);

    // Numberof directories we need to traverse
    int number_of_directiores = slashes + 1;

    for (int i = 0; i < number_of_directiores; i++)
    {
        // Extract next directory name
        std::string output = remove_prefix(dirpath);
        // Find name in current_directorie
        int idx = find_name_in_dir(output);

        // Check that the directorie exists
        if (idx == -2)
        {
            printf(CD_ERROR_NOT_FOUND);
            current_dir = restore;
            return -2;
        }

        // Read current_dir block
        uint8_t data[BLOCK_SIZE];
        disk.read(current_dir.first_blk, data);
        struct dir_entry *entry_arr = (dir_entry *)data;

        // Check so that we don't try to cd in to a file
        if (entry_arr[idx].type == 1)
        {
            current_dir = entry_arr[idx];
        }
        else
        {
            printf(CD_ERROR);
            current_dir = restore;
            return -2;
        }
    }

    return 0;
}
// pwd prints the full path, i.e., from the root directory, to the current
// directory, including the currect directory name
int FS::pwd()
{

    std::string path = pwd2();
    printf("%s", path.c_str());
    printf("\n");
    return 0;
}

// chmod <accessrights> <filepath> changes the access rights for the
// file <filepath> to <accessrights>.
int FS::chmod(std::string accessrights, std::string filepath)
{
    // Convert new accessrights to int
    int access_rights_int = std::stoi(accessrights);

    // Set restore point
    std::string restore_point = pwd2();

    // Extract filename
    std::string file_name = rightmost(filepath);

    // Change current_dir to to where file exist
    cd2(filepath);

    // Check so that file exists
    int idx = find_name_in_dir(file_name);
    if (idx == -2)
    {
        printf(CHMOD_ERROR);
        cd(restore_point);
        return -2;
    }

    // Read current_dir
    uint8_t temp_arr[BLOCK_SIZE];
    struct dir_entry *entry_arr = (struct dir_entry *)temp_arr;
    disk.read(current_dir.first_blk, (uint8_t *)entry_arr);

    // Set new rights
    entry_arr[idx].access_rights = access_rights_int;

    // Write back to disk
    disk.write(current_dir.first_blk, (uint8_t *)entry_arr);
    return 0;
}
