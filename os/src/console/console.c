/*
 * console.c
 *
 *  Created on: 08.05.2015
 *      Author: Nicolaj Hoess
 */

#include "console.h"

#include <stdio.h>
#include "../kernel/kernel.h"

#define CONSOLE_SCREEN_HEIGHT_LINES			(40)
#define CONSOLE_MAX_COMMAND_LENGTH			(255)
#define CONSOLE_SCANF_FORMAT				"%254[^\0]s"

static boolean_t initialized = false;
static device_t consoleDevice;

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

	ConsoleClear();
}

void ConsoleProcess(int argc, char** argv)
{

	if (initialized == false) {
		// TODO Send error message to the kernel (not initialized)
		return;
	}

	ConsoleClear();
	printOSLogo();
	printWelcomeMessage();

	char command[CONSOLE_MAX_COMMAND_LENGTH];
	while(TRUE)
	{
		memset(&command[0], 0, CONSOLE_MAX_COMMAND_LENGTH);

		printPrompt();		// Prompt: root@fhvos#

		// Read the command using scanf (Overriden through fgetc and ungetc)
		scanf(CONSOLE_SCANF_FORMAT, command);
		KernelDebug("Console: Input command received: %s\n", command);

		KernelExecute(command);
	}
}

void printWelcomeMessage()
{
	DeviceManagerWrite(consoleDevice, "Welcome root...\r\nYou are logged in\r\n", 40); // TODO Extract this as a constant
}
void printOSLogo()
{
	unsigned int major, minor, patch;
	KernelVersion(&major, &minor, &patch);
	char versionLine[43];

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

	// TODO If a version number's length is >1 we'll face a formatting issue :)
	sprintf(&versionLine[0], " Kernel v%i.%i.%i            @@@@@@      \r\n", major, minor, patch);
	DeviceManagerWrite(consoleDevice, &versionLine[0], 42);

	DeviceManagerWrite(consoleDevice, "                           @<3@       \r\n", 42);
	DeviceManagerWrite(consoleDevice, "                            @@        \r\n", 42);
}
void printPrompt()
{
	DeviceManagerWrite(consoleDevice, "\r\nroot@fhv-os# ", 15);
}

void ConsoleClear()
{
	char escCommand[1] = { 27 };
	DeviceManagerWrite(consoleDevice, escCommand, 1);       // ESC command
	DeviceManagerWrite(consoleDevice, "[2J", 3);    // Clear screen command
	DeviceManagerWrite(consoleDevice, escCommand, 1);		// ESC command
	DeviceManagerWrite(consoleDevice, "[H", 2);     // Cursor to home command
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
