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

#include "../systemapi/includes/system.h"

#define CONSOLE_SCREEN_HEIGHT_LINES			(40)
#define CONSOLE_MAX_COMMAND_LENGTH			(255)
#define CONSOLE_MAX_HISTORY					(15)
#define CONSOLE_SCANF_FORMAT				"%254[^\r]"

//see http://en.wikipedia.org/wiki/ANSI_escape_code#graphics
#define CONSOLE_COLOR_BLACK				("\e[0;30m")
#define CONSOLE_COLOR_RED				("\e[0;31m")
#define CONSOLE_COLOR_GREEN				("\e[0;32m")
#define CONSOLE_COLOR_YELLOW			("\e[0;33m")
#define CONSOLE_COLOR_BLUE				("\e[0;34m")
#define CONSOLE_COLOR_MAGENTA			("\e[0;35m")
#define CONSOLE_COLOR_CYAN				("\e[0;36m")
#define CONSOLE_COLOR_WHITE				("\e[0;37m")

#define CONSOLE_COLOR_ERROR				(CONSOLE_COLOR_RED)
#define CONSOLE_COLOR_INFO				(CONSOLE_COLOR_WHITE)
#define CONSOLE_COLOR_DEBUG				(CONSOLE_COLOR_GREEN)

static boolean_t initialized = false;
static device_t consoleDevice;

static void printWelcomeMessage();
static void printOSLogo();
static void printPrompt();
static boolean_t acceptChar(char ch);
static void clearCommand(char*);
static void createHistoryEntry(char* command);
static void shiftCursorLeft(int count);

int unchar[2] = { -1, -1 };

typedef struct history_entry_struct history_entry_t;

struct history_entry_struct {
	history_entry_t* prev;
	char command[CONSOLE_MAX_COMMAND_LENGTH];
	history_entry_t* next;
};

static history_entry_t* history_curr;
static history_entry_t* history_head;
static history_entry_t* history_tail;
static unsigned int 	history_count;

void ConsoleInit(device_t device)
{
	consoleDevice = device;

	DeviceManagerInit(consoleDevice);

	DeviceManagerOpen(consoleDevice);

	initialized = true;

	history_count = 0;

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
	int commandPos;
	int commandPosMax;
	char buf;
	while(TRUE)
	{
		memset(&command[0], '\0', CONSOLE_MAX_COMMAND_LENGTH);
		commandPos = 0;
		commandPosMax = 0;
		buf = 0;
		history_curr = NULL;
		boolean_t endLine = false;
		printPrompt();		// Prompt: root@fhvos#

		// Reading the line
		do {
			do {
				DeviceManagerRead(consoleDevice, &buf, 1);
				switch(buf)
				{
					case 27:    // Escape character
					{
						DeviceManagerRead(consoleDevice, &buf, 1);
						switch(buf)
						{
						case 91:// [ character
						{
							DeviceManagerRead(consoleDevice, &buf, 1);
							switch(buf)
							{
							case 68:
								// Left
								if (commandPos >= 1)
								{
									DeviceManagerWrite(consoleDevice, "\x1B\x5B\x44", 3);
									commandPos--;
								}
								break;
							case 67:
								// Right
								if (commandPos < commandPosMax)
								{
									DeviceManagerWrite(consoleDevice, "\x1B\x5B\x43", 3);
									commandPos++;
								}
								break;
							case 66:
								// Down
								if (history_curr != NULL) {
									history_curr = history_curr->next;
									shiftCursorLeft(commandPos);
									DeviceManagerWrite(consoleDevice, "\x1B[K", 3);
									if (history_curr == NULL || history_curr == history_curr->next)
									{
										history_curr = NULL;
										commandPos = 0;
										commandPosMax = 0;
									}
									else
									{
										commandPos = strlen(history_curr->command);
										commandPosMax = 0;
										strcpy(command, history_curr->command);
										DeviceManagerWrite(consoleDevice, command, commandPos);
									}
								}
								break;
							case 65:
								// Up
								if (history_curr == NULL)
								{
									history_curr = history_tail;
								}
								else if (history_curr != history_head)
								{
									history_curr = history_curr->prev;
								}
								if (history_curr != NULL)
								{
									shiftCursorLeft(commandPos);
									DeviceManagerWrite(consoleDevice, "\x1B[K", 3);
									commandPos = strlen(history_curr->command);
									commandPosMax = 0;
									strcpy(command, history_curr->command);
									DeviceManagerWrite(consoleDevice, command, commandPos);
								}
								break;
							}
							break;
						}
						}
						buf = 0;
						break;
					}
					case 127:
						// Backspace
						if (commandPos < 1)
						{
							break;
						}
						DeviceManagerWrite(consoleDevice, "\x7F", 1);
						command[--commandPos] = '\0';
						break;
					case '\r':
					case '\0':
						DeviceManagerWrite(consoleDevice, "\r\n", 2);
						command[commandPosMax++] = '\0';
						endLine = true;
						break;
				}

				commandPosMax = MAX(commandPos, commandPosMax);
			} while (acceptChar(buf) == false && endLine == false);

			if (endLine == false && commandPos < CONSOLE_MAX_COMMAND_LENGTH - 1)
			{
				DeviceManagerWrite(consoleDevice, &buf, 1);
				command[commandPos++] = buf;
			}
			commandPosMax = MAX(commandPos, commandPosMax);

		} while (endLine == false);

		// Line read successfully - perform trivial check and trimming
		clearCommand(command); // Re-ensure that no invalid characters are parsed

		int cLen = strlen(command);
		if (commandPos <= 0 || cLen <= 0)
		{
			KernelDebug("Empty command received\n");
		}
		else
		{
			createHistoryEntry(command);
			KernelDebug("Console: Input command received (length=%i): %s\n", cLen, command);

			// Execute the command through system call
			int pid = execute(command);

			KernelDebug("Return value from execute(..) is=%d\n", pid);
		}
	}
}

void printWelcomeMessage()
{
	ConsoleDisplayInfo();
	DeviceManagerWrite(consoleDevice, "Welcome root...\r\nYou are logged in\r\n\r\n", 38); // TODO Extract this as a constant
}
void printOSLogo()
{
	unsigned int major, minor, patch;
	KernelVersion(&major, &minor, &patch);
	char versionLine[43];

	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_RED, 8);
	DeviceManagerWrite(consoleDevice, "                            @'        \r\n", 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_GREEN, 8);
	DeviceManagerWrite(consoleDevice, "                           @@@'       \r\n", 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_YELLOW, 8);
	DeviceManagerWrite(consoleDevice, "                          @WA@@'      \r\n", 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_BLUE, 8);
	DeviceManagerWrite(consoleDevice, "                         @@@@@@@;     \r\n", 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_MAGENTA, 8);
	DeviceManagerWrite(consoleDevice, "                        @@@AS@@@@'               __           __       \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_CYAN, 8);
	DeviceManagerWrite(consoleDevice, "                         @@@@@PR@@'             /\\ \\         / /\\      \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_WHITE, 8);
	DeviceManagerWrite(consoleDevice, "                         @@@<3@@@@@'           /  \\ \\       / /  \\     \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_RED, 8);
	DeviceManagerWrite(consoleDevice, "                        @@AS@@@@@@@@'         / /\\ \\ \\     / / /\\ \\__  \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_GREEN, 8);
	DeviceManagerWrite(consoleDevice, " @@@+@@  @ @+  @       @@@AW@@@@WA@@@'       / / /\\ \\ \\   / / /\\ \\___\\ \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_YELLOW, 8);
	DeviceManagerWrite(consoleDevice, " @   @@  @ +@ @@      @@@@@'+@@PR@@@@@      / / /  \\ \\_\\  \\ \\ \\ \\/___/ \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_BLUE, 8);
	DeviceManagerWrite(consoleDevice, " @@@.@@@@@  @ @,     @@WA@'  @@@@@@@@      / / /   / / /   \\ \\ \\       \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_MAGENTA, 8);
	DeviceManagerWrite(consoleDevice, " @   @@  @  @'@       @@@'  @@@@@WA@      / / /   / / /_    \\ \\ \\      \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_CYAN, 8);
	DeviceManagerWrite(consoleDevice, " @   @@  @  :@@        @'  @@<3@@@@      / / /___/ / //_/\\__/ / /      \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_WHITE, 8);
	DeviceManagerWrite(consoleDevice, " :   ;:  :   :            @@@WA@@@      / / /____\\/ / \\ \\/___/ /       \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_RED, 8);
	DeviceManagerWrite(consoleDevice, " FHV OS                  @@@@@@@@       \\/_________/   \\_____\\/        \r\n", 75);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_GREEN, 8);
	// FIXME If a version number's length is >1 we'll face a formatting issue :)
	sprintf(&versionLine[0], " Kernel v%i.%i.%i            @@@@@@      \r\n", major, minor, patch);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_YELLOW, 8);
	DeviceManagerWrite(consoleDevice, &versionLine[0], 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_BLUE, 8);
	DeviceManagerWrite(consoleDevice, "                           @<3@       \r\n", 40);
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_MAGENTA, 8);
	DeviceManagerWrite(consoleDevice, "                            @@        \r\n", 40);
}
void printPrompt()
{
	ConsoleDisplayInfo(),
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
	case '~':
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

void createHistoryEntry(char* command)
{
	// Search for duplicate entry
	history_curr = history_head;
	while (history_curr != NULL)
	{
		if (strcmp(history_curr->command, command) == 0)
		{
			// Duplicate
			if (history_head == history_tail)
			{
				free(history_head);
				history_head = NULL;
				history_tail = NULL;
			}
			else if (history_curr == history_head)
			{
				history_head->next->prev = NULL;
				history_head = history_curr->next;
				free(history_curr);
			}
			else if (history_curr == history_tail)
			{
				return;
			}
			else
			{
				history_curr->next->prev = history_curr->prev;
				history_curr->prev->next = history_curr->next;
				free(history_curr);
			}
			history_count--;
			break;
		}
		history_curr = (history_curr == history_curr->next) ? NULL : history_curr->next;
	}

	// Create memory for the history command
	history_entry_t* new = malloc(sizeof(history_entry_t));
	strcpy(&new->command[0], command);

	if (history_head == NULL && history_tail == NULL)
	{
		history_head = new;
		history_tail = new;
		history_head->next = new;
		history_head->prev = NULL;
	}
	else
	{
		new->prev = history_tail;
		new->next = NULL;
		history_tail->next = new;
		history_tail = new;
	}

	history_count++;

	if (history_count > CONSOLE_MAX_HISTORY)
	{
		history_entry_t* temp = history_head->next;
		temp->prev = NULL;
		free(history_head);
		history_head = temp;
		history_count--;
	}
}

void shiftCursorLeft(int count)
{
	if (count <= 0 || count >= CONSOLE_MAX_COMMAND_LENGTH)
	{
		return;
	}
	char buf[10];
	// <ESC>[{COUNT}D
	int res = sprintf(&buf[0], "\x1B\x5B%d\x44", count);
	DeviceManagerWrite(consoleDevice, &buf[0], res);
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

void ConsoleDisplayInfo() {
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_INFO, 8);
}

void ConsoleDisplayDebug() {
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_DEBUG, 8);
}

void ConsoleDisplayError() {
	DeviceManagerWrite(consoleDevice, CONSOLE_COLOR_ERROR, 8);
}
