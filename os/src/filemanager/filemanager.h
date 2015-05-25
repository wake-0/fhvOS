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

#define FILE_MANAGER_OK				(1)
#define FILE_MANAGER_NOT_FOUND		(-1)

#define FILE_MANAGER_BIN_PATH		"bin"

extern int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv, boolean_t blocking, context_t* context);

// TODO Add more functions see Bug #74

#endif /* FILEMANAGER_FILEMANAGER_H_ */
