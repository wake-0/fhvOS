/*
 * filemanager.h
 *
 *  Created on: 19.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef FILEMANAGER_FILEMANAGER_H_
#define FILEMANAGER_FILEMANAGER_H_

#include "../platform/platform.h"
#include "../scheduler/scheduler.h"

#define FILE_MANAGER_OK				  (1)
#define FILE_MANAGER_NOT_FOUND		  (-1)
#define FILE_MANAGER_BUFFER_TOO_SMALL (-2)
#define FILE_MANAGER_NO_DEVICE_FOUND  (-4)

#define FILE_MANAGER_BIN_PATH		"bin"
#define FILE_MANAGER_MAX_FILE_LENGTH (5+1+3+1)

typedef enum { TYPE_DIRECTORY, TYPE_FILE } entryType_t;

typedef struct {
	char name[FILE_MANAGER_MAX_FILE_LENGTH];
	entryType_t type;
} directoryEntry_t;

extern int FileManagerInit(device_t device);
extern int FileManagerListDirectoryContent(const char* name, entryType_t* buf, int length);
extern int FileManagerOpenFile(const char* name, int startByte, char* buf, int length);
extern int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv, boolean_t blocking, context_t* context);

// TODO Add more functions see Bug #74

#endif /* FILEMANAGER_FILEMANAGER_H_ */
