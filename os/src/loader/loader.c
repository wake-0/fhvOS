/*
 * loader.c
 *
 *  Created on: 03.06.2015
 *      Author: Kevin Wallis
 */

#include "loader.h"
#include <stdio.h>
#include "../memmanager/memmanager.h"
#include "../processmanager/processmanager.h"
#include "../hal/cpu/hal_cpu.h"
#include "../hal/am335x/hw_types.h"

static char* progBuf;
static int progBufLen;

void dummyStart(int argc, char** argv)
{
	int length = progBufLen;
	char* programBuf = progBuf;
	int pageSize = PAGE_SIZE_4KB;

	//pageAddressPointer_t pages = MemoryManagerGetFreePagesInProcessRegion(numPages);
	//memcpy(0x10F00000, progBuf, 4096); //programBuf, length);
	//memcpy(0x10F00000 + 4096, progBuf, 4096); //programBuf, length);
	int i = 0;
	void* virt_mem_start = (void*) 0x10020000; // TODO Extract as define
	//for (i = 0; i < progBufLen; i++) {
	//	memcpy((virt_mem_start + i), programBuf + i, 1); //programBuf, length);
		//printf("Copied byte: %d\n", i);
	//}
	memcpy(virt_mem_start, programBuf, progBufLen);

	free(programBuf);
	process_t* proc = SchedulerGetRunningProcess();
	proc->temp_pc = virt_mem_start; // TODO Set PC to main of program

	while(1)
	{

	}
}

int LoaderLoad(char* programBuf, int length)
{

	progBuf = programBuf;
	progBufLen = length;
	// See ticket #82 : pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve)
	process_t* proc = ProcessManagerStartProcess("new", dummyStart, 0, NULL, false, NULL);

	return 0;
}

