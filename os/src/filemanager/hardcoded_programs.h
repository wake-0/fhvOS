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
#include <system.h>

#define	HARDCODED_PROGRAMS_COUNT			(15)
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
void HardCodedPrograms_Uptime(int argc, char** argv);


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
		{ "pong", HardCodedPrograms_Pong },
		{ "uptime", HardCodedPrograms_Uptime }
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

#define IPC_DEVICE_CHANNEL_PREF		"at.fhv.dmxdriver.device"
#define IPC_DEVICE_CHANNEL_PREF_LEN	(24)
#define IPC_CONTROL_CHANNEL		"at.fhv.dmxdriver.control"
#define COMMAND_START			"start"
#define	COMMAND_QUIT			"quit"
#define COMMAND_STATUS			"status"
#define COMMAND_NEW_DEVICE		"add"
#define COMMAND_REMOVE_DEVICE	"remove"
#define MAX_DEVICES				2
#define DMX_CHANNELS			6

static int currDevices = 0;
static char* dmxBuffer = 0;
static int deviceHandle = 0;

static int checkControlChannel();
static void checkDeviceChannels();
static void updateDmx();

void HardCodedPrograms_Dmx(int argc, char** argv) {

	if (!open_ipc_channel(IPC_CONTROL_CHANNEL))
	{
		printf("Error opening ipc channel\n");
		return;// -1;
	}

	// Pre-Start loop
	while(1)
	{
		while (!has_ipc_message(IPC_CONTROL_CHANNEL))
		{
			yield();
		}

		char message[30];
		char sender[IPC_MAX_NAMESPACE_NAME];
		if (get_next_ipc_message(IPC_CONTROL_CHANNEL, message, 30, sender, IPC_MAX_NAMESPACE_NAME))
		{
			if (strcmp(COMMAND_START, message) == 0)
			{
				break;
			}
			else if (strcmp(COMMAND_QUIT, message) == 0)
			{
				close_ipc_channel(IPC_CONTROL_CHANNEL);
				return;// 1;
			}
			else if (strcmp(COMMAND_STATUS, message) == 0)
			{
				send_ipc_message(IPC_CONTROL_CHANNEL, sender, "READY", 8);
			}
			else
			{
				send_ipc_message(IPC_CONTROL_CHANNEL, sender, "KKTHX", 6);
			}
		}
	}

	deviceHandle = open_device("DMX");

	// Control loop
	while(1)
	{
		if (checkControlChannel() == -1)
		{
			break;
		}
		checkDeviceChannels();

		updateDmx();

		sleep(50);
	}

	close_ipc_channel(IPC_CONTROL_CHANNEL);
	int i = 0;
	for (i = 0; i < MAX_DEVICES; i++) {
		char channelBuf[IPC_DEVICE_CHANNEL_PREF_LEN + 1];
		sprintf(channelBuf, "%s%d\0", IPC_DEVICE_CHANNEL_PREF, i);
		close_ipc_channel(channelBuf);
	}

	if (dmxBuffer != NULL && currDevices > 0)
	{
		memset(dmxBuffer, 0, currDevices * DMX_CHANNELS);
		updateDmx();
		updateDmx();
		updateDmx();
	}
	close_device(deviceHandle);
	free(dmxBuffer);
	currDevices = 0;
	return;// 0;
}

int checkControlChannel()
{
	if (!has_ipc_message(IPC_CONTROL_CHANNEL))
	{
		return 0;
	}

	char message[30];
	char sender[IPC_MAX_NAMESPACE_NAME];
	if (get_next_ipc_message(IPC_CONTROL_CHANNEL, message, 30, sender, IPC_MAX_NAMESPACE_NAME))
	{
		if (strcmp(COMMAND_START, message) == 0)
		{
			return 0;
		}
		else if (strcmp(COMMAND_QUIT, message) == 0)
		{
			send_ipc_message(IPC_CONTROL_CHANNEL, sender, "KKTHXBYE", 9);
			return -1;
		}
		else if (strcmp(COMMAND_STATUS, message) == 0)
		{
			send_ipc_message(IPC_CONTROL_CHANNEL, sender, "STARTED", 8);
		}
		else if (strcmp(COMMAND_NEW_DEVICE, message) == 0)
		{
			if (currDevices >= MAX_DEVICES)
			{
				send_ipc_message(IPC_CONTROL_CHANNEL, sender, "TOOMUCH", 8);
				return 1;
			}
			if (currDevices == 0)
			{
				dmxBuffer = malloc(sizeof(char) * DMX_CHANNELS);
			}
			else
			{
				dmxBuffer = realloc(dmxBuffer, sizeof(char) * DMX_CHANNELS * (currDevices + 1));
			}
			if (dmxBuffer != NULL)
			{
				memset(&dmxBuffer[currDevices * DMX_CHANNELS], 0, DMX_CHANNELS);
				char channelBuf[IPC_DEVICE_CHANNEL_PREF_LEN + 1];
				sprintf(channelBuf, "%s%d\0", IPC_DEVICE_CHANNEL_PREF, ++currDevices);
				open_ipc_channel(channelBuf);
				updateDmx();
			}
		}
		else if (strcmp(COMMAND_REMOVE_DEVICE, message) == 0)
		{
			if (currDevices > 0)
			{
				memset(&dmxBuffer[(currDevices - 1) * DMX_CHANNELS], 0, DMX_CHANNELS);
				updateDmx();
				updateDmx();
				updateDmx();
				char channelBuf[IPC_DEVICE_CHANNEL_PREF_LEN + 1];
				sprintf(channelBuf, "%s%d\0", IPC_DEVICE_CHANNEL_PREF, currDevices);
				close_ipc_channel(channelBuf);
				currDevices--;
			}
			if (currDevices == 0)
			{
				free(dmxBuffer);
				dmxBuffer = NULL;
			}
			else
			{
				dmxBuffer = realloc(dmxBuffer, currDevices * DMX_CHANNELS);
			}
		}
		else
		{
			send_ipc_message(IPC_CONTROL_CHANNEL, sender, "KKTHX", 6);
		}
	}
	return 0;
}

void checkDeviceChannels()
{
	int i = 0;
	for (i = 0; i < MAX_DEVICES; i++) {
		char channelBuf[IPC_DEVICE_CHANNEL_PREF_LEN + 1];
		sprintf(channelBuf, "%s%d\0", IPC_DEVICE_CHANNEL_PREF, (i + 1));
		char message[30];
		char sender[IPC_MAX_NAMESPACE_NAME];
		if (has_ipc_message(channelBuf) > 0)
		{
			int res = get_next_ipc_message(channelBuf, message, 30, sender, IPC_MAX_NAMESPACE_NAME);
			if (res)
			{
				// First char is channel, second is value
				int channel = message[0];
				int value = message[1];
				dmxBuffer[i * DMX_CHANNELS + channel] = value;
			}
		}
	}
}

void updateDmx()
{
	if (dmxBuffer != NULL && currDevices > 0)
	{
		ioctl_device(deviceHandle, 0, 0, dmxBuffer, DMX_CHANNELS * currDevices);
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

	send_ipc_message("at.fhv.pong", "at.fhv.ping", argv[0], strlen(argv[0]) + 1);

	close_ipc_channel("at.fhv.pong");
}

void HardCodedPrograms_Uptime(int argc, char** argv)
{
	printf("%d\n", uptime());
}

#endif /* FILEMANAGER_HARDCODED_PROGRAMS_H_ */
