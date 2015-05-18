/*
 * kernel.c
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#include "kernel.h"
#include "../processmanager/processmanager.h"
#include "../processmanager/mutex.h"
#include "../devicemanager/devicemanager.h"
#include "../console/console.h"
#include <stdio.h>

static long uptimeTicks = 0;
static boolean_t started = FALSE;
static mutex_t startMutex;
void KernelStart()
{
	MutexLock(&startMutex);

	if (started == TRUE)
	{
		KernelError("Illegal call of KernelStart()\n");
	}


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

	KernelInfo("Starting Process Manager\n");
	ProcessManagerInit();
	KernelInfo("Process Manager started\n");

	KernelInfo("Starting console\n");
	ProcessManagerStartProcess("tty", &ConsoleProcess);
	KernelInfo("Console Started\n");

	started = TRUE;

	MutexUnlock(&startMutex);

	// Non-returning function
	while(1)
	{

	}
}

long KernelGetUptime()
{
	return uptimeTicks;
}

long KernelTick(int ticks)
{
	return (uptimeTicks += ticks);
}

int	KernelInfo(const char *format, ...)
{
	va_list arg;
	va_start (arg, format);
	int res = 0;
	res += printf("[INFO  @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	va_end(arg);

	return res;
}
int	KernelDebug(const char *format, ...)
{
	va_list arg;
	va_start (arg, format);
	int res = 0;
	res += printf("[DEBUG @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	va_end(arg);

	return res;
}
int	KernelError(const char *format, ...)
{

	va_list arg;
	va_start (arg, format);
	int res = 0;
	res += printf("[ERROR @ %i] ", KernelGetUptime());
	res += vprintf(format, arg);
	va_end(arg);

	return res;
}
