/*
 * mmu.c
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


#include "mmu.h"

#define MMU_DOMAIN_FULL_ACCESS 0xFFFFFFFF

#define MASTER_PAGE_TABLE_SECTION_FULL_ACCESS	0xC02		// AP = 0b11, first two bits are 0b10 for section entry
#define UPPER_12_BITS_MASK						0xFFF00000

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

	mmuInitializeKernelMasterPageTable(masterTable);

	return masterTable;
}


/**
 * \brief	This function fills the master page table which contains the entries for the kernel region.
 * 			It is statically mapped to the addresses 0x80000000 to 0x81000000. For the corret page table entry
 * 			format see ARM Architecture Reference Manual -> "first level descriptor"
 * \return 	none
 */
static void mmuInitializeKernelMasterPageTable(pageTablePointer_t masterPageTable)
{
	unsigned int physicalAddress;
	unsigned int pageTableEntry = 0;
	unsigned int baseAddress = 0;
	pageTablePointer_t table = masterPageTable;

	for(physicalAddress = KERNEL_START_ADDRESS; physicalAddress < KERNEL_END_ADDRESS; physicalAddress += L1_PAGE_TABLE_SIZE_16KB)
	{
		baseAddress = physicalAddress & UPPER_12_BITS_MASK;
		pageTableEntry = baseAddress | MASTER_PAGE_TABLE_SECTION_FULL_ACCESS;
		*table = pageTableEntry;
		table++;
	}
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
