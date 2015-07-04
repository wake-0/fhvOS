/*
 * loader.c
 *
 *  Created on: 03.06.2015
 *      Author: Kevin Wallis
 */

#include "loader.h"
#include <stdio.h>
#include <system.h>
#include "../memmanager/memmanager.h"
#include "../hal/cpu/hal_cpu.h"
#include "../hal/am335x/hw_types.h"

#define	LOADER_PROGRAM_START_ADDRESS	0x10020000

void dummyStart(int argc, char** argv)
{
	process_t* proc = SchedulerGetRunningProcess();
	void* virt_mem_start = (void*) LOADER_PROGRAM_START_ADDRESS;
	memcpy(virt_mem_start, proc->programData, proc->programDataLen);
	free(proc->programData);
	proc->temp_pc = virt_mem_start;
	yield();
}

int LoaderLoad(char* programBuf, int length, boolean_t blocking, context_t* context)
{

	// See ticket #82 : pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve)
	process_t* proc = ProcessManagerStartProcess("new", dummyStart, 0, NULL, blocking, context);

	if (proc != NULL)
	{
		proc->programData = programBuf;
		proc->programDataLen = length;
	}
	return 0;
}

