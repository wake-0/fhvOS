/*
 * filemanager.c
 *
 *  Created on: 19.05.2015
 *      Author: Nicolaj Hoess
 */

#include "filemanager.h"
#include "hardcoded_programs.h"
#include "../processmanager/processmanager.h"

int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv)
{
	if (searchInGlobalBinPath)
	{
		// Search in global bin path first (this includes hardcoded user-processes)
		void (*funcPtr)(int, char ** ) = HardCodedProgramsGetProgram(name);
		if (funcPtr != NULL)
		{
			ProcessManagerStartProcess(name, funcPtr);
			return FILE_MANAGER_OK;
		}
	}

	// TODO Search in current working directory

	return FILE_MANAGER_NOT_FOUND;
}
