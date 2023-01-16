/*
By Jakob Adamsson and Emil Gullbrandsson
*/
#ifndef __ERRORS_H__
#define __ERRORS_H__

// MKDIR
#define FILE_EXISTING_DIR "File already exists, mkdir failed \n"
// Create
#define CREATE_ERROR "To many files, create failed\n"
#define FILE_EXISTING "File already exists, cannot create new file\n"

// CAT
#define CAT_ERROR "File not found, cat failed\n"
#define NOT_A_FILE "Not a file, cat failed\n"
#define NO_READ_ACCESS "No read permission, cat failed\n"

// CP
#define CP_ERROR_DEST "Destination file already existing, cp failed\n"
#define CP_ERROR_SRC "Source file did not exist, cp failed\n"
#define SPACE_ERROR_DEST "To many files in dest, cp failed\n"

// Change current dir
#define CHANGE_CURRENT_DIR_ERROR "Folder not found, change_current_directory failed\n"

// MV
#define SRC_NOT_FOUND_ERROR "Sourcefile were not found, move failed\n"
#define DEST_NOT_FOUND_ERROR "Destpath were not found, move failed\n"
#define DEST_EXISTING "Destpath already exist, move failed\n"

// RM
#define RM_ERROR "File not found, rm failed\n"
#define FILEPATH_NOT_FOUND "Could not find filepath, rm failed\n"

// APPEND
#define FILEPATH1_NOT_FOUND "Filepath1 were not found, append failed\n"
#define FILEPATH2_NOT_FOUND "Filepath2 were not found, append failed\n"
#define NO_READ_ACCESS_FILEPATH1 "No read permission on filepath1, append failed\n"
#define NO_WRITE_ACCESS_FILEPATH2 "No write permission on filepath2, append failed\n"

// CD
#define CD_ERROR "Tried to cd into file, cd failed\n"
#define CD_ERROR_NOT_FOUND "Folder not found, cd failed\n"

// CHMOD
#define CHMOD_ERROR "File not found, chmod failed\n"

// MISC
#define MAX_FILES "Could not create file, max files limit exceeded\n"
#define NAME_TO_LONG_ERROR "Name too long\n"

#endif // endif errors.h