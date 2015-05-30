/*
 * kernel.c
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#include "kernel.h"
#include "../platform/platform.h"
#include "../processmanager/processmanager.h"
#include "../processmanager/mutex.h"
#include "../devicemanager/devicemanager.h"
#include "../console/console.h"
#include "../filemanager/filemanager.h"
#include "../systemapi/includes/system.h"
#include <stdio.h>
#include <string.h>

#define KERNEL_VERSION_MAJOR		(0)
#define KERNEL_VERSION_MINOR		(1)
#define KERNEL_VERSION_PATCH		(0)

#define	KERNEL_MAX_COMMAND_ARGS		(64)

static long uptimeTicks = 0;
static boolean_t started = FALSE;
static mutex_t startMutex;

static void kernelIdleProcess(int argc, char** argv);

void KernelStart()
{
	MutexLock(&startMutex);

	if (started == TRUE)
	{
		KernelError("Illegal call of KernelStart()\n");
	}

	int res = 0;

	DeviceManagerInit();
	device_t uart = DeviceManagerGetDevice("UART0", 5);
	ConsoleInit(uart);
	KernelInfo("Device Manager Started\n");
	KernelInfo("UART0 as console prepared\n");

	KernelInfo("Turning on Power LED\n");
	device_t led3 = DeviceManagerGetDevice("LED3", 4);
	DeviceManagerOpen(led3);
	DeviceManagerWrite(led3, "1", 1);
	KernelInfo("Turned on Power LED\n");

	KernelInfo("Starting File Manager\n");
	device_t sdcard = DeviceManagerGetDevice("SDCARD", 6);
	if ((res = FileManagerInit(sdcard)) == FILE_MANAGER_OK)
	{
		KernelInfo("File Manager started\n");
	}
	else
	{
		KernelInfo("File Manager was not initialized\n");
		KernelError("FileManagerInit returned %d\n", res);
	}

	KernelInfo("Starting Process Manager\n");
	ProcessManagerInit();
	KernelInfo("Process Manager started\n");

	KernelAtomicStart();

	KernelInfo("Starting idle process\n");
	ProcessManagerStartProcess("idle", &kernelIdleProcess, 0, NULL, false, NULL);
	KernelInfo("Idle process started\n");

	KernelInfo("Starting console\n");
	ProcessManagerStartProcess("tty", &ConsoleProcess, 0, NULL, false, NULL);
	KernelInfo("Console Started\n");

	KernelAtomicEnd();

	started = TRUE;

	MutexUnlock(&startMutex);

	// Non-returning function
	while(1)
	{

	}
}

void KernelVersion(unsigned int* major, unsigned int* minor, unsigned int* patch)
{
	*major = KERNEL_VERSION_MAJOR;
	*minor = KERNEL_VERSION_MINOR;
	*patch = KERNEL_VERSION_PATCH;
}

long KernelGetUptime()
{
	return uptimeTicks;
}

long KernelTick(int ticks)
{
	return (uptimeTicks += ticks);
}

void KernelExecute(char* inputCommand, context_t* context)
{
	// Parse the command
	// TODO I'm not sure if we should parse the command and args in this function or directly in the console
	char* argv[KERNEL_MAX_COMMAND_ARGS];
	int argc = 0;
	char* command;

	char* ch;
	ch = strtok(inputCommand, " ");
	command = ch;
	do {
		ch = strtok(NULL, " ");
		argv[argc++] = ch;
	} while (ch != NULL);
	argc--;

	boolean_t blocking = true;
	if (strlen(argv[argc-1]) == 1 && *argv[argc-1] == '&')
	{
		argv[argc-1] = '\0';
		argc--;
		blocking = false;
	}

	KernelDebug("Command=%s and has %i arguments\n", command, argc);

	if (FILE_MANAGER_NOT_FOUND == FileManagerOpenExecutable(command, TRUE, argc, argv, blocking, context))
	{
		printf("Command %s not found\n", command);
	}
}

int	KernelPrint(const char *format, ...)
{
#if	KERNEL_DEFAULT_OUTPUT
	va_list arg;
	va_start (arg, format);
	int res = 0;
	ConsoleColor(WHITE);
	res += vprintf(format, arg);
	va_end(arg);

	return res;
#else
	return 0;
#endif
}

int	KernelInfo(const char *format, ...)
{
#if	KERNEL_INFO_OUTPUT
	va_list arg;
	va_start (arg, format);
	int res = 0;
	ConsoleColor(CYAN);
	res += printf("[INFO  @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	ConsoleColor(WHITE);
	va_end(arg);

	return res;
#else
	return 0;
#endif
}

int	KernelDebug(const char *format, ...)
{
#if KERNEL_DEBUG_OUTPUT
	va_list arg;
	va_start (arg, format);
	int res = 0;
	ConsoleColor(GREEN);
	res += printf("[DEBUG @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	ConsoleColor(WHITE);
	va_end(arg);

	return res;
#else
	return 0;
#endif
}

int	KernelError(const char *format, ...)
{
#if KERNEL_ERROR_OUTPUT
	va_list arg;
	va_start (arg, format);
	int res = 0;
	ConsoleColor(RED);
	res += printf("[ERROR @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	ConsoleColor(WHITE);
	va_end(arg);

	return res;
#else
	return 0;
#endif
}

void KernelAtomicStart()
{
	SchedulerDisableScheduling();
}

void KernelAtomicEnd()
{
	SchedulerEnableScheduling();
}

static void kernelIdleProcess(int argc, char** argv)
{
	while(true)
	{
		yield();
	}
}
