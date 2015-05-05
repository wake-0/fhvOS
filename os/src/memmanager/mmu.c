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
#define BOUNDARY_SELECTION_RANGE				0x7
#define BOUNDARY_AT_HALF_OF_VIRTUAL_MEMORY		0x1

static void mmuReserveAllDirectMappedRegions();
static void mmuReserveDirectMappedRegion(unsigned int memoryRegion);
static void mmuInitializeKernelMasterPageTable(pageTablePointer_t masterPageTable);
static void mmuSetKernelMasterPageTable(pageTablePointer_t table);
static void mmuSetProcessPageTable(pageTablePointer_t table);
static void mmuSetDomainToFullAccess(void);
static pageTablePointer_t mmuCreateMasterPageTable(uint32_t virtualStartAddress, uint32_t virtualEndAddress);
static int mmuGetTableIndex(unsigned int virtualAddress, unsigned int indexType);
static pageTablePointer_t mmuGetL2PageTable(pageTablePointer_t pageTableL1, unsigned int virtualAddress);
static void mmuSetTranslationTableSelectionBoundary(unsigned int selectionBoundary);

// accessed by MMULoadDabtData in coprocessor.asm
volatile uint32_t dabtAccessedAddress;
volatile uint32_t dabtFaultState;

// for testing purposes
// TODO: delete after testing
volatile uint32_t currentAddressInTTBR0;
volatile uint32_t currentAddressInTTBR1;


pageTablePointer_t kernelMasterPageTable;


int MMUInit()
{
	MemoryManagerInit();

	MMUDisable();

	// reserve direct mapped regions
	mmuReserveAllDirectMappedRegions();

	// master page table for kernel region must be created statically and before MMU is enabled
	kernelMasterPageTable = mmuCreateMasterPageTable(KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);

	MMUReadKernelTableAddress();
	currentAddressInTTBR1 = 0;

	mmuSetKernelMasterPageTable(kernelMasterPageTable);

	MMUReadKernelTableAddress();

	mmuSetProcessPageTable(kernelMasterPageTable);

	// set domain access
	mmuSetDomainToFullAccess();

	mmuSetTranslationTableSelectionBoundary(BOUNDARY_AT_HALF_OF_VIRTUAL_MEMORY);

	mmuSetTranslationTableSelectionBoundary(BOUNDARY_AT_HALF_OF_VIRTUAL_MEMORY);

	// TODO: enabling mmu still causes great problems <= son of a bitch
	MMUEnable();

	return MMU_OK;
}

void MMUHandleDataAbortException()
{
	printf("dabt interrupt\n");

	// get mmu data abort details
	dabtAccessedAddress = 0;
	dabtFaultState		= 0;
	MMULoadDabtData();

	// TODO: switch to kernel mode is maybe needed

	// get current process
	process_t* runningProcess = SchedulerGetRunningProcess();

	if(NULL == runningProcess->pageTableL1)
	{
		SchedulerKillProcess(runningProcess->id);
	}

	// TODO: check dabt details

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


/**
 * \brief	Reserves all pages of all regions except page table and process region.
 */
static void mmuReserveAllDirectMappedRegions(void)
{
	unsigned int memoryRegion;

	for(memoryRegion = 0; memoryRegion < MEMORY_REGIONS - 2; memoryRegion++)
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

static void mmuWritePageTableEntryL2(address_t physicalAddress)
{

}


/**
 * \brief	Sets the address of a L1 page table to TTBR1.
 */
static void mmuSetKernelMasterPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	MMUSetKernelTable(table);
}


/**
 * \brief	Sets the address of a L1 page table to TTBR0.
 */
static void mmuSetProcessPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	MMUSetProcessTable(table);
}


static void mmuSetDomainToFullAccess(void)
{
	MMUSetDomainAccess(MMU_DOMAIN_FULL_ACCESS);
}


/**
 * \brief	Writes to TTBCR.N and sets the selection boundary between TTBR0 and TTBR1.
 */
static void mmuSetTranslationTableSelectionBoundary(unsigned int selectionBoundary)
{
	selectionBoundary &= BOUNDARY_SELECTION_RANGE;
	MMUSetTranslationTableControlRegister(selectionBoundary);
}

static pageTablePointer_t mmuGetL2PageTable(pageTablePointer_t pageTableL1, unsigned int virtualAddress)
{
	int tableOffset = mmuGetTableIndex(virtualAddress, INDEX_OF_L2_PAGE_TABLE);

	if(tableOffset < VALID_PAGE_TABLE_OFFSET)
	{
		// no existing L2 page table, create one
		return MemoryManagerCreatePageTable(L2_PAGE_TABLE);
	}
	else
	{
		// L2 page table exists
		pageTablePointer_t l2PageTable = (pageTableL1 + tableOffset);
		return l2PageTable;
	}
}

static int mmuGetTableIndex(unsigned int virtualAddress, unsigned int indexType)
{
	switch(indexType)
	{
		case INDEX_OF_L1_PAGE_TABLE:
			return (virtualAddress & L1_PAGE_TABLE_INDEX_MASK) >> (L2_PAGE_TABLE_INDEX_MASK | PAGE_FRAME_INDEX_MASK);
		case INDEX_OF_L2_PAGE_TABLE:
			return (virtualAddress & L2_PAGE_TABLE_INDEX_MASK) >> PAGE_FRAME_INDEX_MASK;
		case INDEX_OF_PAGE_FRAME:
			return (virtualAddress & PAGE_FRAME_INDEX_MASK);
		default:
			return -1;
	}
}
