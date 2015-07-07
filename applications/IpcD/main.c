#include <stdio.h>
#include <string.h>
#include <ipc.h>
#include <stdlib.h>
#include <system.h>

#define CHANNEL_NAME "at.fhv.ipcdaemon"

void printUsage();
void checkResponse();

int main(int argc, char** argv) {
	if (argc == 0)
	{
		printUsage();
		return 0;
	}

	int res = open_ipc_channel(CHANNEL_NAME);
	if (!res)
	{
		printf("Error opening ipc channel.\n");
		return 0;
	}

	if (argc == 1 && strcmp(argv[0], "-l") == 0)
	{
		ipc_channel_list_entry_t entries[5];
		int count = list_ipc_channels(entries, 5);
		int i = 0;
		printf("I found %d open ipc channels\n", count);
		for (i = 0; i < count; i++) {
			printf("%s\n", entries[i].channel_name);
		}
	}
	else if (argc > 1)
	{
		if (strcmp(argv[0], "-s") == 0)
		{
			// Send string
			if (argc != 3)
			{
				printUsage();
			}
			else
			{
				if (send_ipc_message(CHANNEL_NAME, argv[1], argv[2], strlen(argv[2]) + 1))
				{
					printf("Message %s was sent.\n", argv[2]);
				}

				checkResponse();
			}
		}
		else if (strcmp(argv[0], "-c") == 0)
		{
			if (argc < 3 || argc > 14)
			{
				printUsage();
			}
			else
			{
				char* channel_name = argv[1];
				int i = 2;
				char buf[12];
				for (; i < argc; i++)
				{
					int num = atoi(argv[i]);
					buf[i-2] = (char) num;
				}
				send_ipc_message(CHANNEL_NAME, channel_name, buf, i - 2);

				checkResponse();
			}
		}
	}

	close_ipc_channel(CHANNEL_NAME);
}

void checkResponse()
{
	printf("Waiting 1s for response...\n");
	sleep(1000);
	if (has_ipc_message(CHANNEL_NAME))
	{
		char message[100];
		char sender[IPC_MAX_NAMESPACE_NAME];
		get_next_ipc_message(CHANNEL_NAME, message, 100, sender, IPC_MAX_NAMESPACE_NAME);
		printf("Got response: %s\n", message);
	}
	else
	{
		printf("Did not get any response...\n");
	}
}

void printUsage()
{
	printf("Usage is:\n");
	printf("\t-l\n\t\tList all available ipc channels (actually it's max 5 of them)\n");
	printf("\t-s [channel name] [string]\n\t\tSends the given string to the given channel\n");
	printf("\t-c [channel name] {[num], [num], ...}\n\t\tSends the given numbers (max 12 numbers, as 8 bits) to the given channel\n");
}
