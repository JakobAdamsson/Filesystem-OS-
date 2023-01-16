#include <iostream>
#include <cstdint>
#include "disk.h"

#ifndef __FS_H__
#define __FS_H__

#define ROOT_BLOCK 0
#define FAT_BLOCK 1
#define FAT_FREE 0
#define FAT_EOF -1

#define TYPE_FILE 0
#define TYPE_DIR 1
#define READ 0x04
#define WRITE 0x02
#define EXECUTE 0x01

struct dir_entry
{
    char file_name[56] = ""; // name of the file / sub-directory
    uint32_t size;           // size of the file in bytes
    uint16_t first_blk;      // index in the FAT for the first block of the file
    uint8_t type = 2;        // directory (1) or file (0)
    uint8_t access_rights;   // read (0x04), write (0x02), execute (0x01)
};

class FS
{
private:
    Disk disk;
    // size of a FAT entry is 2 bytes
    int16_t fat[BLOCK_SIZE / 2];
    struct dir_entry current_dir;
    struct dir_entry root_dir;

public:
    FS();
    ~FS();
    // formats the disk, i.e., creates an empty file system
    int format();
    // create <filepath> creates a new file on the disk, the data content is
    // written on the following rows (ended with an empty row)
    int create(std::string filepath);
    // cat <filepath> reads the content of a file and prints it on the screen
    int cat(std::string filepath);
    // ls lists the content in the current directory (files and sub-directories)
    int ls();

    // cp <sourcepath> <destpath> makes an exact copy of the file
    // <sourcepath> to a new file <destpath>
    int cp(std::string sourcepath, std::string destpath);
    // mv <sourcepath> <destpath> renames the file <sourcepath> to the name <destpath>,
    // or moves the file <sourcepath> to the directory <destpath> (if dest is a directory)
    int mv(std::string sourcepath, std::string destpath);
    // rm <filepath> removes / deletes the file <filepath>
    int rm(std::string filepath);
    // append <filepath1> <filepath2> appends the contents of file <filepath1> to
    // the end of file <filepath2>. The file <filepath1> is unchanged.
    int append(std::string filepath1, std::string filepath2);

    // mkdir <dirpath> creates a new sub-directory with the name <dirpath>
    // in the current directory
    int mkdir(std::string dirpath);
    // cd <dirpath> changes the current (working) directory to the directory named <dirpath>
    // Used for when filepath ends with a directory
    int cd(std::string dirpath);
    // pwd prints the full path, i.e., from the root directory, to the current
    // directory, including the current directory name
    int pwd();

    // chmod <accessrights> <filepath> changes the access rights for the
    // file <filepath> to <accessrights>.
    int chmod(std::string accessrights, std::string filepath);

    // Finds the file within directory and inc the size of the current dir
    int find_name_in_dir(std::string filepath);

    // Writes blocks to disk and updates FAT, returns index of first block
    int write_blocks(char *content);

    // Writes to current direcory
    int add_entry(struct dir_entry new_file);

    // Count the amount of entries given a certain folder
    int count_entries();

    // Reads content of file and returns a pointer to the content, caller must free the return pointer.
    char *read_file_content(struct dir_entry file);

    // Removes entry from current_dir
    int remove_entry(int idx);

    // Updates FAT table
    void reset_blocks(int idx, int size);

    // Writes a directory to a block and returns the index of the first block
    int write_dir_to_block(struct dir_entry file);

    // Returns the path of the current directory as a string
    std::string pwd2();

    // Used for when filepath ends with a file
    int cd2(std::string dirpath);
};

#endif // __FS_H__
