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
#define	LOADER_ARGC_ARGV_MEMORY			0x00100000

void dummyStart(int argc, char** argv)
{
	process_t* proc = SchedulerGetRunningProcess();
	char* virt_mem_start = (char*) LOADER_PROGRAM_START_ADDRESS;
	char* argc_argv_mem = (char*) LOADER_ARGC_ARGV_MEMORY;
	*((int*)(&argc_argv_mem[0])) = argc;
	char** curr_argv = (char**) (LOADER_ARGC_ARGV_MEMORY + 0x10);
	char* curr_argv_mem = (char*) (LOADER_ARGC_ARGV_MEMORY + 0x10 + 1 + (255 * sizeof(char))); // TODO Get the constant for max argv from somewhere
	unsigned int i = 0;
	for (i = 0; i < argc; i++)
	{
		curr_argv[i] = curr_argv_mem;
		int len = strlen(argv[i]);
		strncpy(curr_argv_mem, argv[i], len);
		*(curr_argv_mem + len) = '\0';
		curr_argv_mem += len + 1;
	}

	memcpy(virt_mem_start, proc->programData, proc->programDataLen);
	free(proc->programData);
	proc->temp_pc = (address_t*) virt_mem_start;
	yield();
}

int LoaderLoad(char* programBuf, int length, int argc, char** argv, boolean_t blocking, context_t* context)
{

	// See ticket #82 : pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve)
	process_t* proc = ProcessManagerStartProcess("new", dummyStart, argc, argv, blocking, context);

	if (proc != NULL)
	{
		proc->programData = programBuf;
		proc->programDataLen = length;
	}
	return 0;
}

