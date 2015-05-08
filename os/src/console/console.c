/*
 * console.c
 *
 *  Created on: 08.05.2015
 *      Author: Nicolaj Hoess
 */

#include "console.h"

#include <stdio.h>

#define CONSOLE_SCREEN_HEIGHT_LINES			(40)
#define CONSOLE_MAX_COMMAND_LENGTH			(255)
#define ASCII_BACKSPACE						(127)

static boolean_t initialized = false;
static device_t consoleDevice;

static void clearScreen();
static void printWelcomeMessage();
static void printOSLogo();
static void printPrompt();

void ConsoleInit(device_t device)
{
	consoleDevice = device;

	DeviceManagerInit(consoleDevice);

	DeviceManagerOpen(consoleDevice);

	initialized = true;
}

void ConsoleProcess()
{

	if (initialized == false) {
		// TODO Send error message to the kernel (not initialized)
		return;
	}

	clearScreen();
	printOSLogo();
	printWelcomeMessage();

	char command[CONSOLE_MAX_COMMAND_LENGTH];
	while(TRUE)
	{
		printPrompt();		// Prompt: root@fhvos#

		memset(&command[0], '\0', CONSOLE_MAX_COMMAND_LENGTH);
		int commandPos = 0;

		boolean_t commandFound = false;
		char buf[1];
		while(commandFound == false) {
			if (commandPos == CONSOLE_MAX_COMMAND_LENGTH)
			{
				// Send Backspace key
				char bs[1] = { ASCII_BACKSPACE };
				DeviceManagerWrite(consoleDevice, bs , 1);
			}

			DeviceManagerRead(consoleDevice, &buf[0], 1);

			if (buf[0] == '\r')		// If user hit enter
			{
				commandFound = true;
				DeviceManagerWrite(consoleDevice, "\r\n", 2);
			}
			else
			{
				if (buf[0] == ASCII_BACKSPACE && commandPos > 0) // TODO Add handling for more control commands
				{
					commandPos--;
					command[commandPos] = '\0';
				}
				if (commandPos >= 0)
				{
					command[commandPos++] = buf[0];
					DeviceManagerWrite(consoleDevice, buf, 1);
				}
			}
		}

		// TODO Send command to the kernel (IPC)

		// XXX Just for debug purpose
		char debugOutput[CONSOLE_MAX_COMMAND_LENGTH + 50];
		sprintf(&debugOutput[0], "[DEBUG] input command:%s\r\n\r", command);
		DeviceManagerWrite(consoleDevice, debugOutput, CONSOLE_MAX_COMMAND_LENGTH + 50);
		DeviceManagerWrite(consoleDevice, command, CONSOLE_MAX_COMMAND_LENGTH);
		DeviceManagerWrite(consoleDevice, "\r\n", 2);

	}
}

void printWelcomeMessage()
{
	DeviceManagerWrite(consoleDevice, "Welcome root...\r\nYou are logged in\r\n", 40); // TODO Extract this as a constant
}
void printOSLogo()
{
	DeviceManagerWrite(consoleDevice, "                            @'        \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                           @@@'       \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                          @WA@@'      \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                         @@@@@@@;     \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                        @@@AS@@@@'    \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                         @@@@@PR@@'   \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                         @@@<3@@@@@'  \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                        @@AS@@@@@@@@' \r\n", 42);
	DeviceManagerWrite(consoleDevice, " @@@+@@  @ @+  @       @@AAW@@@@WA@@@'\r\n", 42);
	DeviceManagerWrite(consoleDevice, " @   @@  @ +@ @@      @@@@@'+@@PR@@@@@\r\n", 42);
	DeviceManagerWrite(consoleDevice, " @@@.@@@@@  @ @,     @@WA@'  @@@@@@@@ \r\n", 42);
	DeviceManagerWrite(consoleDevice, " @   @@  @  @'@       @@@'  @@@@@WA@  \r\n", 42);
	DeviceManagerWrite(consoleDevice, " @   @@  @  :@@        @'  @@<3@@@@   \r\n", 42);
	DeviceManagerWrite(consoleDevice, " :   ;:  :   :            @@@WA@@@    \r\n", 42);
	DeviceManagerWrite(consoleDevice, " FHV OS                  @@@@@@@@     \r\n", 42);
	DeviceManagerWrite(consoleDevice, " Kernel v0.0.1            @@@@@@      \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                           @<3@       \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                            @@        \r\n", 42);
}
void printPrompt()
{
	DeviceManagerWrite(consoleDevice, "\r\nroot@fhv-os# ", 15);
}

void clearScreen()
{
	int i = 0;
	for (i = 0; i < CONSOLE_SCREEN_HEIGHT_LINES; i++)
	{
		DeviceManagerWrite(consoleDevice, "                                      \r\n", 42);
	}
}
