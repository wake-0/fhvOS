/*
 * console.c
 *
 *  Created on: 08.05.2015
 *      Author: Nicolaj Hoess
 */

#include "console.h"

#include <stdio.h>
#include <ctype.h>
#include "../kernel/kernel.h"
#include "../systemapi/systemcalls.h"

#define CONSOLE_SCREEN_HEIGHT_LINES			(40)
#define CONSOLE_MAX_COMMAND_LENGTH			(255)
#define CONSOLE_SCANF_FORMAT				"%254[^\r]"

static boolean_t initialized = false;
static device_t consoleDevice;

static void printWelcomeMessage();
static void printOSLogo();
static void printPrompt();
static boolean_t acceptChar(char ch);
static void clearCommand(char*);

int unchar[2] = { -1, -1 };

void ConsoleInit(device_t device)
{
	consoleDevice = device;

	DeviceManagerInit(consoleDevice);

	DeviceManagerOpen(consoleDevice);

	initialized = true;

	ConsoleClear();
}

void ConsoleProcess(int argc, char** argv)
{
	if (initialized == false) {
		KernelError("Console not initialized before started as process\n");
		return;
	}

	ConsoleClear();
	printOSLogo();
	printWelcomeMessage();

	char command[CONSOLE_MAX_COMMAND_LENGTH];
	while(TRUE)
	{
		memset(&command[0], '\0', CONSOLE_MAX_COMMAND_LENGTH);

		printPrompt();		// Prompt: root@fhvos#

		// Read the command using scanf (Overriden through fgetc and ungetc)
		int res = scanf(CONSOLE_SCANF_FORMAT, command);
		clearCommand(command); // Re-ensure that no invalid characters are parsed

		int cLen = strlen(command);
		if (res <= 0 || cLen <= 0)
		{
			KernelDebug("Empty command received\n");
		}
		else
		{
			KernelDebug("Console: Input command received (length=%i): %s\n", cLen, command);

			// System call
			systemCallMessage_t message;
			message.systemCallNumber = SYSTEM_CALL_EXEC;
			message.messageArgs.forwardArg = CONSOLE_MAX_COMMAND_LENGTH;
			message.messageArgs.buf = command;

			SystemCall(&message);
		}
	}
}

void printWelcomeMessage()
{
	DeviceManagerWrite(consoleDevice, "Welcome root...\r\nYou are logged in\r\n\r\n", 38); // TODO Extract this as a constant
}
void printOSLogo()
{
	unsigned int major, minor, patch;
	KernelVersion(&major, &minor, &patch);
	char versionLine[43];

	DeviceManagerWrite(consoleDevice, "                            @'        \r\n", 40);
	DeviceManagerWrite(consoleDevice, "                           @@@'       \r\n", 40);
	DeviceManagerWrite(consoleDevice, "                          @WA@@'      \r\n", 40);
	DeviceManagerWrite(consoleDevice, "                         @@@@@@@;     \r\n", 40);
	DeviceManagerWrite(consoleDevice, "                        @@@AS@@@@'               __           __       \r\n", 75);
	DeviceManagerWrite(consoleDevice, "                         @@@@@PR@@'             /\\ \\         / /\\      \r\n", 75);
	DeviceManagerWrite(consoleDevice, "                         @@@<3@@@@@'           /  \\ \\       / /  \\     \r\n", 75);
	DeviceManagerWrite(consoleDevice, "                        @@AS@@@@@@@@'         / /\\ \\ \\     / / /\\ \\__  \r\n", 75);
	DeviceManagerWrite(consoleDevice, " @@@+@@  @ @+  @       @@@AW@@@@WA@@@'       / / /\\ \\ \\   / / /\\ \\___\\ \r\n", 75);
	DeviceManagerWrite(consoleDevice, " @   @@  @ +@ @@      @@@@@'+@@PR@@@@@      / / /  \\ \\_\\  \\ \\ \\ \\/___/ \r\n", 75);
	DeviceManagerWrite(consoleDevice, " @@@.@@@@@  @ @,     @@WA@'  @@@@@@@@      / / /   / / /   \\ \\ \\       \r\n", 75);
	DeviceManagerWrite(consoleDevice, " @   @@  @  @'@       @@@'  @@@@@WA@      / / /   / / /_    \\ \\ \\      \r\n", 75);
	DeviceManagerWrite(consoleDevice, " @   @@  @  :@@        @'  @@<3@@@@      / / /___/ / //_/\\__/ / /      \r\n", 75);
	DeviceManagerWrite(consoleDevice, " :   ;:  :   :            @@@WA@@@      / / /____\\/ / \\ \\/___/ /       \r\n", 75);
	DeviceManagerWrite(consoleDevice, " FHV OS                  @@@@@@@@       \\/_________/   \\_____\\/        \r\n", 75);

	// FIXME If a version number's length is >1 we'll face a formatting issue :)
	sprintf(&versionLine[0], " Kernel v%i.%i.%i            @@@@@@      \r\n", major, minor, patch);
	DeviceManagerWrite(consoleDevice, &versionLine[0], 40);

	DeviceManagerWrite(consoleDevice, "                           @<3@       \r\n", 40);
	DeviceManagerWrite(consoleDevice, "                            @@        \r\n", 40);
}
void printPrompt()
{
	DeviceManagerWrite(consoleDevice, "root@fhv-os# ", 13);
}

void ConsoleClear()
{
	char escCommand[1] = { 27 };
	DeviceManagerWrite(consoleDevice, escCommand, 1);       // ESC command
	DeviceManagerWrite(consoleDevice, "[2J", 3);    // Clear screen command
	DeviceManagerWrite(consoleDevice, escCommand, 1);		// ESC command
	DeviceManagerWrite(consoleDevice, "[H", 2);     // Cursor to home command
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
	case '!':
	case '§':
	case '$':
	case '%':
	case '&':
	case '(':
	case ')':
	case '=':
	case '-':
	case '_':
	case '\\':
	case '<':
	case '>':
		return true;
	}
	return false;
}

static void clearCommand(char* command)
{
	// Trim leading/trailing whitespaces
	char* curr = command;
	if (acceptChar(*curr) == FALSE) *curr = ' ';
	int l = strlen(command);

    while(isspace(curr[l - 1])) curr[--l] = 0;
    while(*curr && isspace(*curr)) ++curr, --l;

    memmove(command, curr, l + 1);
}

/*
 * Override for printf and scanf family
 * This is not a clean retargeting but works for the moment.
 */

FILE __stdout =     { 0 };
FILE __stdin =      { 1 };

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

int fgetc(FILE *f)
{
    int c;

    if (unchar[f->fd] == -1)
    {
    	char buf;
        do {
    		DeviceManagerRead(consoleDevice, &buf, 1);
    		if (buf == '\r' || buf == '\0')
    		{
    			DeviceManagerWrite(consoleDevice, "\r\n", 2);
    			f->buf = 0;
    			f->pos = 0;
    			return EOF;
    		}
    	} while (acceptChar(buf) == false);
    	DeviceManagerWrite(consoleDevice, &buf, 1);
    	f->pos++;
        c = buf;
    }
    else
    {
        c = unchar[f->fd];
        unchar[f->fd] = -1;
    }
    return c;
}

int ungetc(int c, FILE *f)
{
    unsigned char uc = c;
    unchar[f->fd] = (int )uc;
    if (uc == '\r')
    	return EOF;
    return (int )uc;
}
