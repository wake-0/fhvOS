/*
 * filemanager.c
 *
 *  Created on: 19.05.2015
 *      Author: Nicolaj Hoess
 */

#include "filemanager.h"
#include "hardcoded_programs.h"
#include "../processmanager/processmanager.h"
#include "../systemapi/includes/system.h"

int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv, boolean_t blocking, context_t* context)
{
	if (searchInGlobalBinPath)
	{
		// Search in global bin path first (this includes hardcoded user-processes)
		void (*funcPtr)(int, char ** ) = HardCodedProgramsGetProgram(name);
		if (funcPtr != NULL)
		{
			process_t* proc = ProcessManagerStartProcess(name, funcPtr, blocking, context);
			return FILE_MANAGER_OK;
		}
	}

	// TODO Search in current working directory

	return FILE_MANAGER_NOT_FOUND;
}

int FileManagerInit(device_t device) {
	return DeviceManagerInitDevice(device);
}

int FileManagerListDirectoryContent(const char* name, entryType_t* buf, int length) {
	return FILE_MANAGER_OK;
}

int FileManagerOpenFile(const char* name, int startByte, char* buf, int length) {
	return FILE_MANAGER_OK;
}
