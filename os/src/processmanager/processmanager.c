/*
 * processmanager.c
 *
 *  Created on: 14.05.2015
 *      Author: Marko
 */

#include "processmanager.h"
#include <string.h>
#include <stdlib.h>
#include "../scheduler/scheduler.h"
#include "../hal/cpu/hal_cpu.h"
#include "../driver/cpu/driver_cpu.h"
#include "../memmanager/mmu.h"
#include "../kernel/kernel.h"

static processInfo_t processes[PROCESSES_MAX];
static int processIdx = 0;


void ProcessManagerInit(void)
{
	SchedulerInit();
	MMUInit();

	device_t timer = DeviceManagerGetDevice("TIMER2", 6);

	// Set up the timer
	DeviceManagerInitDevice(timer);

	device_t cpu = DeviceManagerGetDevice("CPU", 3);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE, 0, NULL, 0);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC, 0, NULL, 0);

	SchedulerStart(timer);
}


process_t* ProcessManagerStartProcess(char * processName, void(*funcPtr)(int, char ** ), int argc, char** argv, boolean_t blocking, context_t* context)
{
	// Create new process info
	process_t* ptr = SchedulerStartProcess(funcPtr);
	if (ptr == NULL)
	{
		return NULL;
	}

	// Add argc and argv to the process (TODO Maybe we have to copy at least argv somehwere into the process's space)
	ptr->context->registers[0] = (register_t) argc;
	char** argv_cpy = malloc(sizeof(char*) * argc);
	unsigned int i = 0;
	for (i = 0; i < argc; i++)
	{
		int len = strlen(argv[i]);
		argv_cpy[i] = malloc(sizeof(char) * len);
		strncpy(argv_cpy[i], argv[i], len + 1);
	}
	ptr->context->registers[1] = (register_t) argv_cpy;

	processes[processIdx].processScheduler = ptr;
	processes[processIdx].processName = malloc(strlen(processName) * sizeof(char));
	strcpy(processes[processIdx].processName, processName);
	processes[processIdx].startTime = KernelGetUptime();

	SchedulerUnblockProcess(ptr->id);

	if (blocking && context != NULL)
	{
		ptr->blockedState = TRUE;
		SchedulerBlockProcess(ptr->parent->id);
		SchedulerRunNextProcess(context);
	}

	return ptr;
}

void ProcessManagerKillProcess(int processId)
{
	// TODO
}


int ProcessManagerGetRunningProcessesCount(void)
{
	return processIdx;
}


void ProcessManagerListProcesses(processInfoAPI_t* processAPIPtr, int length)
{
	// TODO
}

