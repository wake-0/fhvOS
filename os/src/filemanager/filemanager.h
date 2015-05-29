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
#include "../systemapi/includes/system.h"
#include "../systemapi/includes/filesystem.h"

#define FILE_MANAGER_OK				  (1)
#define FILE_MANAGER_NOT_FOUND		  (-1)
#define FILE_MANAGER_BUFFER_TOO_SMALL (-2)
#define FILE_MANAGER_NO_DEVICE_FOUND  (-4)

#define FILE_MANAGER_ROOT_PATH		"/"
#define FILE_MANAGER_BIN_PATH		FILE_MANAGER_ROOT_PATH "bin"

extern int FileManagerInit(device_t device);
extern int FileManagerListDirectoryContent(const char* name, directoryEntry_t* buf, int length);
extern int FileManagerOpenFile(const char* name, int startByte, char* buf, int length);
extern int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv, boolean_t blocking, context_t* context);
extern int FileManagerSetCurrentWorkingDirectory(char *name);
extern int FileManagerGetCurrentWorkingDirectory(char *buf, int len);

// TODO Add more functions see Bug #74

#endif /* FILEMANAGER_FILEMANAGER_H_ */
