/*
 * mmu.c
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


#include "mmu.h"
#include "../scheduler/scheduler.h"
#include "memmanager.h"

static uint32_t* mmuCreateMasterPageTable();
static void mmuReserveAllDirectMappedRegions();
static void mmuReserveDirectMappedRegion(unsigned int memoryRegion);
static void mmuInitializeKernelMasterPageTable();
static void mmuSetKernelMasterPageTable(uint32_t* table);
static void mmuSetProcessPageTable(uint32_t* table);

uint32_t * kernelMasterPageTable;

void MMUInit()
{
	MemoryManagerInit();

	MMUDisable();

	// reserve direct mapped regions
	mmuReserveAllDirectMappedRegions();

	// create master table
	kernelMasterPageTable = mmuCreateMasterPageTable();

	// initialize the kernel master table so the lookup works for the kernel


	// set kernel table

	// set process table

	// set domain access

	// enable mmu
	MMUEnable();
}

static void mmuReserveAllDirectMappedRegions()
{
	unsigned int memoryRegion;

	for(memoryRegion = 0; memoryRegion < MEMORY_REGIONS; memoryRegion++)
	{
		mmuReserveDirectMappedRegion(memoryRegion);
	}
}

static void mmuReserveDirectMappedRegion(unsigned int memoryRegion)
{
	memoryRegionPointer_t region = MemoryManagerGetSection(memoryRegion);

	if(TRUE == region->directAccess)
	{
		MemoryManagerReserveAllPages(region);
	}
}

static uint32_t* mmuCreateMasterPageTable()
{
	// reserve a 16kB pagetable in any region
	uint32_t* newTable;
	unsigned int memoryRegion;

	for(memoryRegion = 0; memoryRegion < MEMORY_REGIONS; memoryRegion++)
	{
		newTable = (uint32_t*)MemoryManagerGetFreePagesInSection(memoryRegion, MMU_MASTER_TABLE_PAGE_COUNT);

		if(NULL != newTable)
		{
			break;
		}
	}

	if(NULL == newTable)
	{
		return NULL;
	}


	// set values of the regin to 0
	memset(newTable, 0, PAGE_SIZE * MMU_MASTER_TABLE_PAGE_COUNT);

	return newTable;
}


static void mmuInitializeKernelMasterPageTable()
{

}

static void mmuSetKernelMasterPageTable(uint32_t* table)
{

}

static void mmuSetProcessPageTable(uint32_t* table)
{

}



void MMUHandleDataAbortException()
{

}


void MMUSwitchToProcess(process_t process)
{

}


void MMUInitProcess(process_t process)
{

}
