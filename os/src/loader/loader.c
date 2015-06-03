/*
 * loader.c
 *
 *  Created on: 03.06.2015
 *      Author: Kevin Wallis
 */

#include "loader.h"
#include "../memmanager/memmanager.h"
#include "../processmanager/processmanager.h"

static char* progBuf;
static int progBufLen;

void dummyStart(int argc, char** argv)
{
	int length = progBufLen;
	char* programBuf = progBuf;
	int pageSize = PAGE_SIZE_4KB;
	int numPages = (length / pageSize) + 1;

	//pageAddressPointer_t pages = MemoryManagerGetFreePagesInProcessRegion(numPages);
	memcpy(0x10F00000, programBuf, length);
	free(programBuf);
	process_t* proc = SchedulerGetRunningProcess();
	proc->context->pc = 0x0000;

}

int LoaderLoad(char* programBuf, int length)
{

	progBuf = programBuf;
	progBufLen = length;
	// See ticket #82 : pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve)
	process_t* proc = ProcessManagerStartProcess("new", dummyStart, 0, NULL, false, NULL);

	return 0;
}

