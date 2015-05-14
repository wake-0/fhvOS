/*
 * processmanager.c
 *
 *  Created on: 14.05.2015
 *      Author: Marko
 */

#include "processmanager.h"
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


void ProcessManagerStartProcess(char * processName, void(*funcPtr)(int, char ** ))
{
	// Create new process info
	process_t ptr = SchedulerStartProcess(funcPtr); // TODO Add argc and argv
	if (ptr == NULL)
	{
		// TODO Add return value
		return;
	}
	MMUInitProcess(ptr);
	processes[processIdx]->processScheduler = ptr;
	strcpy(processes[processIdx]->processName, processName);
	processes[processIdx]->startTime = KernelGetUptime();

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

void ProcessManagerSetProcessState(int processId, processState_t processState)
{
	// TODO
}

