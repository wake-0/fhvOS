/*
 * mmu.c
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


#include "mmu.h"

#define MMU_DOMAIN_FULL_ACCESS 0xFFFFFFFF


static void mmuReserveAllDirectMappedRegions();
static void mmuReserveDirectMappedRegion(unsigned int memoryRegion);
static void mmuInitializeKernelMasterPageTable(pageTablePointer_t masterPageTable);
static void mmuSetKernelMasterPageTable(pageTablePointer_t table);
static void mmuSetProcessPageTable(pageTablePointer_t table);
static void mmuSetDomainToFullAccess(void);
static pageTablePointer_t mmuCreateMasterPageTable(uint32_t virtualStartAddress, uint32_t virtualEndAddress);


pageTablePointer_t kernelMasterPageTable;


int MMUInit()
{
	MemoryManagerInit();

	MMUDisable();

	// reserve direct mapped regions
	mmuReserveAllDirectMappedRegions();

	// master page table for kernel region must be created statically and before MMU is enabled
	kernelMasterPageTable = mmuCreateMasterPageTable(KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);

	// TODO: set kernel table
	mmuSetKernelMasterPageTable(kernelMasterPageTable);

	// TODO: set process table
	mmuSetProcessPageTable(kernelMasterPageTable);

	// set domain access
	mmuSetDomainToFullAccess();

	// enable mmu
	MMUEnable();

	return MMU_OK;
}

void MMUHandleDataAbortException()
{
	// switch to kernel mode is needed

	// TODO: get mmu data function

	// check if L1 page table exists

	// check if L2 page table exists

	// if no L2 page table exists, create one
}

/**
 * \brief	Switch process by setting new L1 page table to ttbr0.
 * 			Cache and TLB will be flushed.
 * \return 	OK if successful, NOT OK else.
 */
int MMUSwitchToProcess(process_t* process)
{
	if(NULL == process->pageTableL1)
	{
		return MMU_NOT_OK;
	}

	// flush TLB and cache, load new process table to ttbr0
	mmuSetProcessPageTable(process->pageTableL1);

	return MMU_OK;
}

// TODO: reconsider principle!
int MMUInitProcess(process_t* process)
{
	// create L1 page table for process
	return MMU_OK;
}


static void mmuReserveAllDirectMappedRegions(void)
{
	unsigned int memoryRegion;

	for(memoryRegion = 0; memoryRegion < MEMORY_REGIONS; memoryRegion++)
	{
		mmuReserveDirectMappedRegion(memoryRegion);
	}
}

static void mmuReserveDirectMappedRegion(unsigned int memoryRegion)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(memoryRegion);

	if(TRUE == region->directAccess)
	{
		MemoryManagerReserveAllPages(region);
	}
}


/**
 * \brief	Creates a master page table for the kernel region.
 * 			Maps statically physical and virtual addresses
 * \return 	Address of page table if successful.
 */
static pageTablePointer_t mmuCreateMasterPageTable(uint32_t virtualStartAddress, uint32_t virtualEndAddress)
{
	pageTablePointer_t masterTable = MemoryManagerCreatePageTable(L1_PAGE_TABLE);

	// TODO: fill page tables with entries so that the lookup works for the kernel
	mmuInitializeKernelMasterPageTable(masterTable);

	return masterTable;
}


// TODO: fill page tables with entries so that the lookup works for the kernel
static void mmuInitializeKernelMasterPageTable(pageTablePointer_t masterPageTable)
{

}

// TODO
static void mmuSetKernelMasterPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	//MMUSetProcessTable();
}

// TODO
static void mmuSetProcessPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	//MMUSetProcessTable();
}


static void mmuSetDomainToFullAccess(void)
{
	MMUSetDomainAccess(MMU_DOMAIN_FULL_ACCESS);
}
