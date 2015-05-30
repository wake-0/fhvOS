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
#include "../systemapi/includes/process.h"
#include "../systemapi/includes/filesystem.h"

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
void HardCodedPrograms_More(int, char**);
void HardCodedPrograms_Ls(int, char**);
void HardCodedPrograms_Cd(int argc, char** argv);
void HardCodedPrograms_Ps(int argc, char** argv);

static command_program_entry_t mapping[HARDCODED_PROGRAMS_COUNT] = {
		{ "hello" , HardCodedPrograms_HelloWorld },
		{ "cwd", HardCodedPrograms_Cwd },
		{ "more", HardCodedPrograms_More },
		{ "ls", HardCodedPrograms_Ls },
		{ "cd", HardCodedPrograms_Cd },
		{ "ps", HardCodedPrograms_Ps }
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
	read_cwd(buf, 255);
	printf("%s\n", buf);
}

void HardCodedPrograms_More(int argc, char** argv)
{
	if (argc != 1)
	{
		printf("No input file given\n");
		return;
	}
	if (strlen(argv[0]) > 13)
	{
		printf("Filename too long\n");
		return;
	}
	char cwd[255-8] = { 0 };
	FileManagerGetCurrentWorkingDirectory(cwd, 255-8);

	char buf[255] = {0};
	if (cwd[strlen(cwd) - 1] == '/')
	{
		sprintf(buf, "%s%s\0", cwd, argv[0]);
	}
	else
	{
		sprintf(buf, "%s/%s\0", cwd, argv[0]);
	}

	char fileBuf[1024] = {0};
	//FileManagerListDirectoryContent(NULL, NULL, 10);
	//if (FileManagerOpenFile(buf, 0, &fileBuf, 1024) == FILE_MANAGER_OK)
	if (read_file(buf, 0, fileBuf, 1024) > 0)
	{
		printf("%s\n", fileBuf);
	}
	else
	{
		printf("Error reading file\n");
	}
}

void HardCodedPrograms_Ls(int argc, char** argv)
{
	char cwd[255-8] = { 0 };
	FileManagerGetCurrentWorkingDirectory(cwd, 255-8);

	directoryEntry_t* retBuf = malloc(sizeof(directoryEntry_t) * 30);
	memset(retBuf, 0, sizeof(directoryEntry_t) * 30);

	if (read_directory(cwd, retBuf, 30) > 0)
	{
		printf("\n");
		int i = 0;
		int files = 0;
		int bytes = 0;
		for (i = 0; i < 30; i++)
		{
			if (strlen(retBuf[i].name) == 0)
			{
				break;
			}
			if (retBuf[i].type == TYPE_FILE)
			{
				files++;
				bytes += retBuf[i].size;
			}
			printf("\t <%c> \t %d \t %s \n", (retBuf[i].type == TYPE_DIRECTORY) ? 'D' : 'F', retBuf[i].size, retBuf[i].name);
		}
		printf("\n %d Entries total", i);
		printf("\n %d Directories", i - files);
		printf("\n %d Files \t %d\n\n", files, bytes);
	}
	else
	{
		printf("Error reading directory\n");
	}
}

void HardCodedPrograms_Cd(int argc, char** argv)
{
	if (argc != 1)
	{
		return;
	}

	char cwd[255-8] = { 0 };
	FileManagerGetCurrentWorkingDirectory(cwd, 255-8);

	char buf[255] = {0};
	if (strcmp("..", argv[0]) == 0)
	{
		int i = strlen(cwd) - 1;
		for (; i > 0 && cwd[i] != '/'; i--)
		{
			cwd[i] = '\0';
		}
		cwd[i] = (i > 0) ? '\0' : cwd[i];
		strncpy(buf, cwd, i+1);
	}
	else
	{
		if (cwd[strlen(cwd) - 1] == '/')
		{
			sprintf(buf, "%s%s\0", cwd, argv[0]);
		}
		else
		{
			sprintf(buf, "%s/%s\0", cwd, argv[0]);
		}
	}

	int res = set_cwd(buf);
	if (res <= 0)
	{
		printf("Could not open directory\n");
	}
}

void HardCodedPrograms_Ps(int argc, char** argv)
{
	int count = get_process_count();

	printf("\nRunning processes: %d\n", count);

	processInfoAPI_t* buf = malloc(sizeof(processInfoAPI_t) * count);

	count = read_processes(buf, count);

	int i = 0;
	printf("\t STATE \t PID \t STIME \t NAME\n");
	for (i = 0; i < count; i++)
	{
		char state = 'R'; // Running and Ready
		switch (buf[i].state) {
			case SLEEPING:
				state = 'S';
				break;
			case BLOCKED:
				state = 'B';
				break;
			default:
				break;
		}
		printf("\t %c \t %d \t %d \t %s\n", state, buf[i].processID, buf[i].startTime, buf[i].processName);
	}
	printf("\n");

	free (buf);
}

#endif /* FILEMANAGER_HARDCODED_PROGRAMS_H_ */
