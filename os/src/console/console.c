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

static boolean_t initialized = false;
static device_t consoleDevice;

static void clearScreen();
static void printWelcomeMessage();
static void printOSLogo();
static void printPrompt();
static boolean_t acceptChar(char ch);

static char ungetBuf;

void ConsoleInit(device_t device)
{
	consoleDevice = device;

	DeviceManagerInit(consoleDevice);

	DeviceManagerOpen(consoleDevice);

	ungetBuf = 0;

	initialized = true;
}

void ConsoleProcess(int argc, char** argv)
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
		memset(&command[0], 0, CONSOLE_MAX_COMMAND_LENGTH);

		printPrompt();		// Prompt: root@fhvos#

		// Read the command using scanf (Overriden through fgetc and ungetc)
		scanf("%s", command); // TODO Only allow a string length of CONSOLE_MAX_COMMAND_LENGTH-1

		// TODO Send command to the kernel (IPC)

		// XXX Just for debug purpose
		char debugOutput[CONSOLE_MAX_COMMAND_LENGTH + 50] = { 0 };
		sprintf(&debugOutput[0], "[DEBUG] input command:%s\r\n", command);
		DeviceManagerWrite(consoleDevice, debugOutput, CONSOLE_MAX_COMMAND_LENGTH + 50);
		printf("[DEBUG] Using printf command:%s\r\n", command);
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

int fputc(int ch, FILE *f)
{
    char tempch = ch;
    switch (tempch) {
    case '\n':
    	DeviceManagerWrite(consoleDevice, "\r\n", 2);
    	break;
    default:
		DeviceManagerWrite(consoleDevice, &tempch, 1);
    	break;
    }
    return ch;
}

int fputs(const char *s, FILE *f)
{
	int cnt = 0;
	char tmpChar = s[cnt];
	while (tmpChar != '\0') {
		DeviceManagerWrite(consoleDevice, &tmpChar, 1);
		cnt++;
		tmpChar = s[cnt];
	}
	return 0;
}

int fgetc(FILE *p)
{
	// TODO Add a switch case over *p (only if we read from stdin we should run the following code)
	if (ungetBuf != 0)
	{
		char temp = ungetBuf;
		ungetBuf = 0;
		return temp;
	}
	char buf = 0;
	do {
		DeviceManagerRead(consoleDevice, &buf, 1);
		if (buf == '\r' || buf == '\0')
		{
			DeviceManagerWrite(consoleDevice, "\r\n", 2);
			return '\0';
		}
	} while (acceptChar(buf) == false);
	// Output the character
	DeviceManagerWrite(consoleDevice, &buf, 1);
	return buf;
}

int ungetc(int c, FILE *p)
{
	return (ungetBuf = c);
}

static boolean_t acceptChar(char ch)
{
	switch (ch)
	{
	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '0' ... '9':
	case '/':
	case ' ':
	case '*':
	case '|':
	case '"':
	case '.':
		return true;
	}
	return false;
}
