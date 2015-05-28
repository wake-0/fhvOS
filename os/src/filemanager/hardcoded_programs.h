/*
 * hardcoded_programs.h
 *
 *  Created on: 19.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef FILEMANAGER_HARDCODED_PROGRAMS_H_
#define FILEMANAGER_HARDCODED_PROGRAMS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	HARDCODED_PROGRAMS_COUNT			(10)
#define HARDCODED_PROGRAMS_MAX_NAME_LEN		(50)

typedef struct {
	char commandName[HARDCODED_PROGRAMS_MAX_NAME_LEN];
	void (*funcPtr)(int, char ** );
} command_program_entry_t;

/*
 * Forward Declarations of test program functions
 */
void HardCodedPrograms_HelloWorld(int, char**);
void HardCodedPrograms_Cwd(int, char**);

static command_program_entry_t mapping[HARDCODED_PROGRAMS_COUNT] = {
		{ "hello" , HardCodedPrograms_HelloWorld },
		{ "cwd", HardCodedPrograms_Cwd }
};

void (*HardCodedProgramsGetProgram(char* name))(int, char**)
{
	int len = strlen(name);
	if (name == NULL || len <= 0 || len >= HARDCODED_PROGRAMS_MAX_NAME_LEN - 1)
	{
		return NULL;
	}
	unsigned int i = 0;
	for (i = 0; i < HARDCODED_PROGRAMS_COUNT; i++)
	{
		if (strcmp(mapping[i].commandName, name) == 0)
		{
			return mapping[i].funcPtr;
		}
	}
	return NULL;
}

/*
 * Definitions of some hardcoded test programs
 */
void HardCodedPrograms_HelloWorld(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Hello %s!\n", argv[0]);
	}
	else
	{
		printf("Hello World!\n");
	}
}

void HardCodedPrograms_Cwd(int argc, char** argv)
{
	char buf[255] = { 0 };
	FileManagerGetCurrentWorkingDirectory(buf, 255);
	printf("%s\n", buf);
}

#endif /* FILEMANAGER_HARDCODED_PROGRAMS_H_ */
