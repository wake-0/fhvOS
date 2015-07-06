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
#include <process.h>
#include <filesystem.h>
#include <devices.h>
#include <ipc.h>

#define	HARDCODED_PROGRAMS_COUNT			(10)
#define HARDCODED_PROGRAMS_MAX_NAME_LEN		(50)

// TODO: this defines should be removed to the dmx function
#define DMX_MAX 6 // Max. number of DMX data packages.

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
void HardCodedPrograms_Kill(int argc, char** argv);
void HardCodedPrograms_Dmx(int argc, char** argv);
void HardCodedPrograms_Ping(int argc, char** argv);
void HardCodedPrograms_Pong(int argc, char** argv);


static command_program_entry_t mapping[HARDCODED_PROGRAMS_COUNT] = {
		{ "hello" , HardCodedPrograms_HelloWorld },
		{ "cwd", HardCodedPrograms_Cwd },
		{ "more", HardCodedPrograms_More },
		{ "ls", HardCodedPrograms_Ls },
		{ "cd", HardCodedPrograms_Cd },
		{ "ps", HardCodedPrograms_Ps },
		{ "dmx", HardCodedPrograms_Dmx },
		{ "kill", HardCodedPrograms_Kill },
		{ "ping", HardCodedPrograms_Ping },
		{ "pong", HardCodedPrograms_Pong }
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
		printf("%s\n", fileBuf);
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
	free(retBuf);
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

void HardCodedPrograms_Dmx(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage is dmx [channel] [value]\n");
		return;
	}

	//int channel = atoi(argv[0]);
	//int value = atoi(argv[1]);

	int handle = open_device("DMX");

	char DMXBuffer[DMX_MAX];
	// Set the default dmx buffer
	int i = 0;
	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	// set tilt
	DMXBuffer[1] = 150;
	// open
	DMXBuffer[3] = 5;
	// set gradient
	DMXBuffer[2] = 125;

	volatile int pan = 0;
	volatile int direction = 0;

	while (direction != 2) {
		// sample stuff
		if (direction == 0) {
			DMXBuffer[pan] += 5;
			if (DMXBuffer[pan] == 255) {
				direction = 1;
			}
		} else if (direction == 1) {
			DMXBuffer[pan] -= 5;
			if (DMXBuffer[pan] == 0) {
				direction = 2;
			}
		}

		for (i = 0; i < DMX_MAX; i++) {
			ioctl_device(handle,0,0, &DMXBuffer[0], sizeof(DMXBuffer));
			//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
			sleep(30);
		}
		if (DMXBuffer[pan] == 0 || DMXBuffer[pan] == 255) {
			printf("Sent DMX Buffer\n");
		}
	}

	sleep(100);

	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	DMXBuffer[0] = 0;
	DMXBuffer[1] = 150;

	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	printf("Sent DMX Buffer 1\n");

	sleep(500);
	DMXBuffer[0] = 127;
	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	printf("Sent DMX Buffer 2\n");

	sleep(500);
	// close
	DMXBuffer[3] = 0;
	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	printf("Sent DMX Buffer 3\n");

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

void HardCodedPrograms_Kill(int argc, char** argv)
{
	if (argc != 1)
	{
		printf("Unknown process id\n");
		return;
	}

	char *end;
	long value = strtol(argv[0], &end, 10);
	if (end == argv[0] || *end != '\0')
	{
		printf("Invalid process id\n");
		return;
	}

	int pid = (int) value;

	if (pid >= 0)
	{
		printf("Kill signal was sent.\n");
		if (kill_process(pid) >= 0)
		{
			printf("Signal was accepted.\n");
		}
		else
		{
			printf("Signal was not accepted.\n");
		}
	}
}

void HardCodedPrograms_Ping(int argc, char** argv)
{
	open_ipc_channel("at.fhv.ping");

	while (has_ipc_message("at.fhv.ping") == FALSE)
	{
		yield();
	}

	char message[10] = { '\0' };
	char sender[20];

	if (get_next_ipc_message("at.fhv.ping", message, 10, sender, 20))
	{
		printf("I got a message from %s:\n-----------------\n%s\n------------------\n", sender, message);
	}
	else
	{
		printf("I got a message but failed to read\n");
	}

	close_ipc_channel("at.fhv.ping");
}

void HardCodedPrograms_Pong(int argc, char** argv)
{
	if (argc != 1)
	{
		printf("Usage is: pong [message]\n");
		return;
	}
	open_ipc_channel("at.fhv.pong");

	send_ipc_message("at.fhv.pong", "at.fhv.ping", argv[0], strlen(argv[0]));

	close_ipc_channel("at.fhv.pong");
}

#endif /* FILEMANAGER_HARDCODED_PROGRAMS_H_ */
