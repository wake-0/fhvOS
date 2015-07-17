#include <stdio.h>
#include <devices.h>
#include <systemcall.h>
#include <system.h>
#include <ipc.h>
#include <string.h>

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

int main(int argc, char** argv) {

	if (!open_ipc_channel(IPC_CONTROL_CHANNEL))
	{
		printf("Error opening ipc channel\n");
		return -1;
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
		sprintf(channelBuf, "%s%d\0", IPC_DEVICE_CHANNEL_PREF, (i+1));
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
	return 0;
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
