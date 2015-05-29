/*
 * filesystem.h
 *
 *  Created on: 29.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef SYSTEMAPI_INCLUDES_FILESYSTEM_H_
#define SYSTEMAPI_INCLUDES_FILESYSTEM_H_

#define FILE_MANAGER_MAX_FILE_LENGTH (5+1+3+1)

typedef enum { TYPE_DIRECTORY, TYPE_FILE } entryType_t;

typedef struct {
	char name[FILE_MANAGER_MAX_FILE_LENGTH];
	unsigned int size;
	entryType_t type;
} directoryEntry_t;

extern int read_cwd(char* buf, int len);
extern int read_file(char* name, int start, char* buf, int len);
extern int read_directory(char* name, directoryEntry_t* buf, int len);

#endif /* SYSTEMAPI_INCLUDES_FILESYSTEM_H_ */
