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
#define UPPER_22_BITS_MASK						0xFFFFFC00
#define UPPER_20_BITS_MASK						0xFFFFF000
#define TTBRC_N									BOUNDARY_AT_HALF_OF_VIRTUAL_MEMORY
#define FAULT_STATUS_MASK_BITS_0_TO_3			0xF
#define FAULT_STATUS_MASK_BIT_4					0x400
#define L1_INDEX_POSITION_IN_VIRTUAL_ADDRESS	20
#define L2_INDEX_POSITION_IN_VIRTUAL_ADDRESS	12
#define TTBR1_BASE_ADDRESS_MASK					0xFFFFC000
#define TTBR0_BASE_ADDRESS_MASK					(0xFFFFC000 >> TTBRC_N) | 0xFFFFC000
#define NUMBER_OF_PAGE_TABLE_PAGE_FRAMES		((0x814FFFFF - 0x81000000) / 0x1000)
#define BIT_MAP_LENGTH							(((SDRAM_END_ADDRESS - PAGE_TABLES_START_ADDRESS) / PAGE_SIZE_4KB) / 8)
#define BIT_MAP_LENGTH_FOR_PAGE_TABLES			(((0x814FFFFF - 0x81000000) / 0x1000) / 8)
#define PAGE_FRAME_NOT_FOUND					0
#define PAGE_FRAME_STATUS_MASK					1
#define SET_PAGE_FRAME_IS_FREE					0
#define SET_PAGE_FRAME_IS_USED					1
#define L1_PAGE_TABLE_ENTRIES					4096
#define L2_PAGE_TABLE_ENTRIES					256
#define SECTION_BASE_POSITION					20
#define SECTION_PAGE_TABLE_MASK					22
#define PAGE_TABLE_BASE_POSITION				10
#define AP_L1_POSITION							10
#define AP_L2_POSITION							4
#define CB_POSITION								2
#define DOMAIN_POSITION							5
#define SMALL_PAGE_BASE_MASK					0xFFFFF000
#define SMALL_PAGE_BASE_POSITION				12

// cache and buffer attributes
#define	NON_CACHED_NON_BUFFERED	            	0x0
#define	NON_CACHED_BUFFERED	            		0x1
#define	WRITE_THROUGH	            			0x2
#define	WRITE_BACK					            0x3

// domain attributes
#define DOMAIN_NO_ACCESS						0x0
#define DOMAIN_CLIENT_ACCESS					0x1
#define DOMAIN_MANAGER_ACCESS					0x3

// access permission settings, see p. B3-1358
#define AP_FULL_ACCESS							0x3
#define AP_NO_ACCESS							0x0

// MMU FAULT STATUS VALUES
#define ALIGNMENT_FAULT							0x1
#define FIRST_LEVEL_TRANSLATION_FAULT			0x5
#define SECOND_LEVEL_TRANSLATION_FAULT			0x7
#define FIRST_LEVEL_PERMISSION_FAULT			0xD
#define SECOND_LEVEL_PERMISSION_FAULT			0xF
#define FIRST_LEVEL_DOMAIN_FAULT				0x9
#define SECOND_LEVEL_DOMAIN_FAULT				0xB
#define TLB_CONFLICT_ABORT						0x10
#define DEBUG_EVENT								0x2
#define	SYNCHRONOUS_EXTERNAL_ABORT				0x8


static void mmuInitializeKernelMasterPageTable(void);
static void mmuSetKernelMasterPageTable(pageTablePointer_t table);
static void mmuSetProcessPageTable(pageTablePointer_t table);
static void mmuSetDomainToFullAccess(void);
static void mmuCreateMasterPageTable(uint32_t virtualStartAddress, uint32_t virtualEndAddress);
static uint32_t mmuGetTableIndex(unsigned int virtualAddress, unsigned int indexType, unsigned int ttbrType);
static pageTablePointer_t mmuGetAddressSpecificL2PageTable(pageTablePointer_t pageTableL1, unsigned int virtualAddress);
static void mmuSetTranslationTableSelectionBoundary(unsigned int selectionBoundary);
static unsigned int mmuGetFaultStatus(void);
static address_t mmuGetFreePageFrameForProcess(void);
static void mmuCreateAndFillL2PageTable(unsigned int virtualAddress, process_t* runningProcess);
static void mmuMapFreePageFrameIntoL2PageTable(unsigned int virtualAddress, pageTablePointer_t l2PageTable);
static void mmuSetPageFrameUsageStatus(unsigned int pageFrameNumber, unsigned int pageFrameStatus);
static address_t mmuGetAddressOfPageFrameNumber(unsigned int pageFrameNumber);
static void freeAllPageFramesOfL2PageTable(pageTablePointer_t l2PageTable);
static void mmuMapDirectRegionToKernelMasterPageTable(memoryRegionPointer_t memoryRegion, pageTablePointer_t table);
static unsigned int mmuCreateL1PageTableEntry(firstLevelDescriptor_t PTE);
static unsigned int mmuCreateL2PageTableEntry(secondLevelDescriptor_t PTE);
static address_t mmuGetFreePageFrameForPageTable(unsigned int pageFramesToReserve);




static pageAddressPointer_t mmuCreatePageTable(unsigned int pageTableType);


// accessed by MMULoadDabtData in coprocessor.asm
volatile uint32_t dabtAccessedVirtualAddress;
volatile uint32_t dabtFaultStatusRegisterValue;

// bitmap used for allocating page frames
static char pageFramesBitMap[BIT_MAP_LENGTH];

// for testing purposes
// TODO: delete after testing
volatile uint32_t currentAddressInTTBR0;
volatile uint32_t currentAddressInTTBR1;
volatile uint32_t currentStatusInSCTLR;
volatile uint32_t currentStatusInTTBCR;

// master L1 page table for statical mapping of kernel, I/O and boot rom
pageTablePointer_t kernelMasterPageTable;


/**
 * \brief	Initialises the MMU for using. This includes creating a master page table and mapping it into the corresponding TTBR register.
 */
int MMUInit()
{
	MemoryManagerInit();

	MMUDisable();

	// reserve direct mapped regions so no accidently reserving of pages can occur
	MemoryManagerReserveAllDirectMappedRegions();

	// master page table for kernel region must be created statically and before MMU is enabled
	mmuCreateMasterPageTable(KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	mmuSetKernelMasterPageTable(kernelMasterPageTable);
	mmuSetProcessPageTable(kernelMasterPageTable);

	// MMU Settings
	mmuSetTranslationTableSelectionBoundary(BOUNDARY_AT_QUARTER_OF_MEMORY);
	mmuSetDomainToFullAccess();

	MMUEnable();

	return MMU_OK;
}


/**
 * \brief	Handles page faults of the MMU. Is called by the assembler function dabt_handler in interrupt.asm
 */
void MMUHandleDataAbortException(context_t* context)
{
	printf("dabt interrupt\n");

	// get mmu data abort details
	dabtAccessedVirtualAddress 		= 0;
	dabtFaultStatusRegisterValue	= 0;
	MMULoadDabtData();

	process_t* runningProcess = SchedulerGetRunningProcess();


	if(NULL == runningProcess)
	{
		// TODO: define where the context is located on the stack and put starting address into R0
		SchedulerRunNextProcess(context);
		return;
	}
	else if(NULL == runningProcess->pageTableL1)
	{
		SchedulerKillProcess(runningProcess->id);
		// TODO: what happens after killing a process? which process is next?
		return;
	}

	unsigned int faultState = mmuGetFaultStatus();

	switch(faultState)
	{
		case ALIGNMENT_FAULT:
			printf("Alignment fault! \n");
			break;
		case FIRST_LEVEL_TRANSLATION_FAULT:
			// no L2 page table
			mmuCreateAndFillL2PageTable(dabtAccessedVirtualAddress, runningProcess);
			break;
		case SECOND_LEVEL_TRANSLATION_FAULT:
			// no page frame
			//pageTablePointer_t l2PageTable = mmuGetAddressSpecificL2PageTable(runningProcess->pageTableL1, dabtAccessedVirtualAddress);
			mmuMapFreePageFrameIntoL2PageTable(dabtAccessedVirtualAddress, mmuGetAddressSpecificL2PageTable(runningProcess->pageTableL1, dabtAccessedVirtualAddress));
			break;
		case FIRST_LEVEL_PERMISSION_FAULT:
			SchedulerKillProcess(runningProcess->id);
			break;
		case SECOND_LEVEL_PERMISSION_FAULT:
			SchedulerKillProcess(runningProcess->id);
			break;
		case DEBUG_EVENT:
			break;
		default:
			break;
	}
}


/**
 * \brief	Returns the current fault state out of the DFSR register value. see p. B4-1561
 */
static unsigned int mmuGetFaultStatus(void)
{
	return ((dabtFaultStatusRegisterValue & FAULT_STATUS_MASK_BIT_4) >> 6)
			| (dabtFaultStatusRegisterValue & FAULT_STATUS_MASK_BITS_0_TO_3);
}


/**
 * \brief	Creates a L2 page table and writes it into the L1 page table of the process.
 *			It finds a free page frame and writes it into the new L2 page table.
 */
static void mmuCreateAndFillL2PageTable(unsigned int virtualAddress, process_t* runningProcess)
{
	// create a L2 page table and write it into L1 page table
	pageTablePointer_t newL2PageTable = mmuCreatePageTable(L2_PAGE_TABLE);
	unsigned int tableIndex = mmuGetTableIndex(virtualAddress, INDEX_OF_L1_PAGE_TABLE, TTBR0);

	firstLevelDescriptor_t pageTableEntry;
	pageTableEntry.sectionBaseAddress 	= (unsigned int)newL2PageTable & SECTION_PAGE_TABLE_MASK;
	pageTableEntry.descriptorType 		= DESCRIPTOR_TYPE_PAGE_TABLE;
	pageTableEntry.cachedBuffered 		= WRITE_BACK;
	pageTableEntry.domain 				= DOMAIN_MANAGER_ACCESS;

	*(runningProcess->pageTableL1 + tableIndex) = mmuCreateL1PageTableEntry(pageTableEntry);

	mmuMapFreePageFrameIntoL2PageTable(virtualAddress, newL2PageTable);
}


/**
 * \brief	Function finds a free page frame and writes it into the new L2 page table.
 */
static void mmuMapFreePageFrameIntoL2PageTable(unsigned int virtualAddress, pageTablePointer_t l2PageTable)
{
	// TODO: handle the case no free page frames are left
	address_t freePageFrame = mmuGetFreePageFrameForProcess();

	secondLevelDescriptor_t pageTableEntry;
	pageTableEntry.pageBaseAddress 	= freePageFrame & SMALL_PAGE_BASE_MASK;
	pageTableEntry.descriptorType  	= DESCRIPTOR_TYPE_SMALL_PAGE;
	pageTableEntry.accessPermission = AP_FULL_ACCESS;
	pageTableEntry.cachedBuffered 	= WRITE_BACK;

	// write into table
	unsigned int tableIndex = mmuGetTableIndex(virtualAddress, INDEX_OF_L2_PAGE_TABLE, TTBR0);
	*(l2PageTable + tableIndex) = mmuCreateL2PageTableEntry(pageTableEntry);
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


/**
 * \brief	Initializes a process by creating and assigning a L1 page table to it.
 * \return 	OK if successful
 */
int MMUInitProcess(process_t* process)
{
	pageTablePointer_t l1PageTable = mmuCreatePageTable(L1_PAGE_TABLE);
	process->pageTableL1 = l1PageTable;
	return MMU_OK;
}


/**
 * \brief	Sets all page frames of a process to unused, when process is being killed.
 */
int MMUFreeAllPageFramesOfProcess(process_t* process)
{
	// TODO: IMPLEMENT!!!!
	unsigned int pageTableEntry;

	for(pageTableEntry = 0; pageTableEntry < L1_PAGE_TABLE_ENTRIES; pageTableEntry++)
	{
		pageTablePointer_t l2PageTable = (pageTablePointer_t)*(process->pageTableL1 + pageTableEntry);
		freeAllPageFramesOfL2PageTable(l2PageTable);
	}

	return MMU_OK;
}


// TODO: implement
static void freeAllPageFramesOfL2PageTable(pageTablePointer_t l2PageTable)
{
	unsigned int pageTableEntry;

	for(pageTableEntry = 0; pageTableEntry < L2_PAGE_TABLE_ENTRIES; pageTableEntry++)
	{
		unsigned int l2PageTableEntry = (unsigned int)*(l2PageTable + pageTableEntry);
		// TODO: finish
	}
}


/**
 * \brief	Creates a master page table for the kernel region.
 * 			Maps statically physical and virtual addresses
 * \return 	Address of page table if successful.
 */
static void mmuCreateMasterPageTable(uint32_t virtualStartAddress, uint32_t virtualEndAddress)
{
	if(NULL != kernelMasterPageTable)
	{
		return;
	}

	kernelMasterPageTable = mmuCreatePageTable(L1_PAGE_TABLE);
	mmuInitializeKernelMasterPageTable();
}


/**
 * \brief	This function fills the master page table which contains the entries for the kernel, boot and I/O region.
 * 			It is statically mapped to the addresses 0x40000000 to 0x814FFFFF. For the corret page table entry
 * 			format see ARM Architecture Reference Manual -> "first level descriptor"
 * \return 	none
 */
static void mmuInitializeKernelMasterPageTable(void)
{
	pageTablePointer_t table = kernelMasterPageTable;

	memoryRegionPointer_t memoryRegion = MemoryManagerGetRegion(BOOT_ROM_REGION);
	mmuMapDirectRegionToKernelMasterPageTable(memoryRegion, table);

	memoryRegion = MemoryManagerGetRegion(MEMORY_MAPPED_IO_REGION);
	mmuMapDirectRegionToKernelMasterPageTable(memoryRegion, table);

	memoryRegion = MemoryManagerGetRegion(KERNEL_REGION);
	mmuMapDirectRegionToKernelMasterPageTable(memoryRegion, table);

	memoryRegion = MemoryManagerGetRegion(PAGE_TABLE_REGION);
	mmuMapDirectRegionToKernelMasterPageTable(memoryRegion, table);
}


/**
 * \brief	This function maps one region directly into the master L1 page table.
 */
static void mmuMapDirectRegionToKernelMasterPageTable(memoryRegionPointer_t memoryRegion, pageTablePointer_t table)
{
	unsigned int physicalAddress;
	firstLevelDescriptor_t pageTableEntry;

	for(physicalAddress = memoryRegion->startAddress; physicalAddress < memoryRegion->endAddress; physicalAddress += 0x100000)
	{
		pageTableEntry.sectionBaseAddress 	= physicalAddress & UPPER_12_BITS_MASK;
		pageTableEntry.descriptorType 		= DESCRIPTOR_TYPE_SECTION;
		pageTableEntry.cachedBuffered 		= WRITE_BACK;
		pageTableEntry.accessPermission 	= AP_FULL_ACCESS;
		pageTableEntry.domain 				= DOMAIN_MANAGER_ACCESS;

		uint32_t tableOffset = mmuGetTableIndex(physicalAddress, INDEX_OF_L1_PAGE_TABLE, TTBR1);

		// see Format of first-level Descriptor on p. B3-1335 in ARM Architecture Reference Manual ARMv7 edition
		uint32_t *firstLevelDescriptorAddress = table + (tableOffset << 2)/sizeof(uint32_t);
		*firstLevelDescriptorAddress = mmuCreateL1PageTableEntry(pageTableEntry);
	}
}


/**
 * \brief	Creates a 32 bit page table entry from a first-level descriptor struct.
 */
static unsigned int mmuCreateL1PageTableEntry(firstLevelDescriptor_t PTE)
{
	unsigned int entry = (PTE.accessPermission << AP_L1_POSITION) | (PTE.domain << DOMAIN_POSITION)
			| (PTE.cachedBuffered << CB_POSITION) | PTE.descriptorType;

	switch(PTE.descriptorType)
	{
		case DESCRIPTOR_TYPE_SECTION:
			entry |= (PTE.sectionBaseAddress &   UPPER_12_BITS_MASK);
			break;
		case DESCRIPTOR_TYPE_PAGE_TABLE:
			entry |= (PTE.sectionBaseAddress & UPPER_22_BITS_MASK);
			break;
		default:
			return FAULT_PAGE_TABLE_ENTRY;
	}

	return entry;
}


/**
 * \brief	Creates a 32 bit page table entry from a second-level descriptor struct.
 */
static unsigned int mmuCreateL2PageTableEntry(secondLevelDescriptor_t PTE)
{
	unsigned int entry = (PTE.accessPermission << AP_L2_POSITION)
					| (PTE.cachedBuffered << CB_POSITION) | PTE.descriptorType;

	switch(PTE.descriptorType)
	{
		case DESCRIPTOR_TYPE_SMALL_PAGE:
			entry |= (PTE.pageBaseAddress & UPPER_20_BITS_MASK);
			break;
		case DESCRIPTOR_TYPE_LARGE_PAGE:
			return FAULT_PAGE_TABLE_ENTRY;		// TODO: implement if needed
		default:
			return FAULT_PAGE_TABLE_ENTRY;
	}

	return entry;
}


/**
 * \brief	Checks the status of a page frame in the bits map.
 */
static int mmuPageFrameIsUsed(unsigned int pageFrameNumber)
{
	return (pageFramesBitMap[pageFrameNumber/8] >> (pageFrameNumber % 8)) & 0x1;
}


/**
 * \brief	Gets a free page frame, and sets its status to used;
 * 			Returns physical address of page frame if successful, NULL otherwise.
 */
static address_t mmuGetFreePageFrameForProcess(void)
{
	unsigned int bitMapByte;
	unsigned int pageNumberOfByte;
	unsigned int pageFrameNumber;

	for(bitMapByte = BIT_MAP_LENGTH_FOR_PAGE_TABLES + 1; bitMapByte < sizeof(pageFramesBitMap); bitMapByte++)
	{
		for(pageNumberOfByte = 0; pageNumberOfByte < 8; pageNumberOfByte++)
		{
			pageFrameNumber = ((bitMapByte * 8) + pageNumberOfByte);

			if(!mmuPageFrameIsUsed(pageFrameNumber))
			{
				break;
			}
			else
			{
				pageFrameNumber = 0;
			}
		}

		if(pageFrameNumber != 0)
		{
			break;
		}
	}

	if(PAGE_FRAME_NOT_FOUND == pageFrameNumber)
	{
		printf("\nPROCESS MEMORY SPACE USED UP!\n");
		return NULL;
	}

	mmuSetPageFrameUsageStatus(pageFrameNumber, SET_PAGE_FRAME_IS_USED);
	return mmuGetAddressOfPageFrameNumber(pageFrameNumber);
}


/**
 * \brief	Allocates the needed space for L1 and L2 page tables.
 * 			Returns physical start address of page table if successful, NULL otherwise.
 */
static address_t mmuGetFreePageFrameForPageTable(unsigned int pageFramesToReserve)
{
	unsigned int bitMapByte;
	unsigned int pageNumberOfByte;

	for(bitMapByte = 0; bitMapByte < BIT_MAP_LENGTH_FOR_PAGE_TABLES; bitMapByte++)
	{
		for(pageNumberOfByte = 0; pageNumberOfByte < 8; pageNumberOfByte++)
		{
			unsigned int pageFrameNumber = ((bitMapByte * 8) + pageNumberOfByte);

			if(NUMBER_OF_PAGE_TABLE_PAGE_FRAMES < pageFrameNumber)
			{
				return NULL;
			}
			else if(!mmuPageFrameIsUsed(pageFrameNumber))
			{
				if(pageFramesToReserve == 1)
				{
					mmuSetPageFrameUsageStatus(pageFrameNumber, SET_PAGE_FRAME_IS_USED);
					return mmuGetAddressOfPageFrameNumber(pageFrameNumber);
				}
				else if((mmuGetAddressOfPageFrameNumber(pageFrameNumber) % L1_PAGE_TABLE_SIZE_16KB) != 0)
				{
					// check for 16 kB alignment failed
					continue;
				}

				// check consecutive page frames
				if(!mmuPageFrameIsUsed(pageFrameNumber + 1) && !mmuPageFrameIsUsed(pageFrameNumber + 2) && !mmuPageFrameIsUsed(pageFrameNumber + 3))
				{
					mmuSetPageFrameUsageStatus(pageFrameNumber, SET_PAGE_FRAME_IS_USED);
					mmuSetPageFrameUsageStatus(pageFrameNumber + 1, SET_PAGE_FRAME_IS_USED);
					mmuSetPageFrameUsageStatus(pageFrameNumber + 2, SET_PAGE_FRAME_IS_USED);
					mmuSetPageFrameUsageStatus(pageFrameNumber + 3, SET_PAGE_FRAME_IS_USED);
					return mmuGetAddressOfPageFrameNumber(pageFrameNumber);
				}
				else
				{
					pageNumberOfByte += 3;
				}
			}
		}
	}
	return NULL;
}

static pageAddressPointer_t mmuCreatePageTable(unsigned int pageTableType)
{
	pageAddressPointer_t tableStartAddress = 0;
	unsigned int pagesToReserve = 0;

	switch(pageTableType)
	{
		case L1_PAGE_TABLE:
			pagesToReserve = L1_TABLE_PAGE_COUNT;			// 16kB
			break;
		case L2_PAGE_TABLE:
			pagesToReserve = L2_TABLE_PAGE_COUNT;			// 1kB
			break;
		default:
			return NULL;
	}

	tableStartAddress = (pageAddressPointer_t)mmuGetFreePageFrameForPageTable(pagesToReserve);

	if(NULL == tableStartAddress)
	{
		return NULL;
	}

	memset(tableStartAddress, FAULT_PAGE_TABLE_ENTRY, PAGE_SIZE_4KB * pagesToReserve);
	return tableStartAddress;
}


/**
 * \brief	Sets the status of a page frame to free(0) or used(1).
 */
static void mmuSetPageFrameUsageStatus(unsigned int pageFrameNumber, unsigned int pageFrameStatus)
{
	char bitMapByte = pageFramesBitMap[pageFrameNumber/8];

	switch(pageFrameStatus)
	{
		case SET_PAGE_FRAME_IS_USED:
			bitMapByte |= (pageFrameStatus & PAGE_FRAME_STATUS_MASK) << (pageFrameNumber % 8);
			break;
		case SET_PAGE_FRAME_IS_FREE:
			bitMapByte &= ~(pageFrameStatus & PAGE_FRAME_STATUS_MASK) << (pageFrameNumber % 8);
			break;
	}

	pageFramesBitMap[pageFrameNumber/8] = bitMapByte;
}


/**
 * \brief	Returns the physical address of a page frame number.
 */
static address_t mmuGetAddressOfPageFrameNumber(unsigned int pageFrameNumber)
{
	if((PROCESS_MEMORY_SPACE + PAGE_TABLE_SPACE) < pageFrameNumber + PAGE_SIZE_4KB)
	{
		return NULL;
	}
	else
	{
		// physical address = start address of page table region in physical memory + number of pages times page frame size
		return PAGE_TABLES_START_ADDRESS + PAGE_SIZE_4KB * pageFrameNumber;
	}
}


/**
 * \brief	Sets the address of a L1 page table to TTBR1.
 */
static void mmuSetKernelMasterPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	MMUSetKernelTable((uint32_t)table);
}


/**
 * \brief	Sets the address of a L1 page table to TTBR0.
 */
static void mmuSetProcessPageTable(pageTablePointer_t table)
{
	MMUFlushTLB();
	MMUSetProcessTable((uint32_t)table);
}


/**
 * \brief	Sets all domain in the coprocessor register to manager mode(full access without permission checking).
 */
static void mmuSetDomainToFullAccess(void)
{
	MMUSetDomainAccess(MMU_DOMAIN_FULL_ACCESS);
}


/**
 * \brief	Writes to TTBCR.N and sets the selection boundary between TTBR0 and TTBR1.
 */
static void mmuSetTranslationTableSelectionBoundary(unsigned int selectionBoundary)
{
	selectionBoundary &= BOUNDARY_SELECTION_MASK;
	MMUSetTranslationTableControlRegister(selectionBoundary);
}


/**
 * \brief	Returns the address of a L2 page table. If none is existing, it creates one.
 */
static pageTablePointer_t mmuGetAddressSpecificL2PageTable(pageTablePointer_t pageTableL1, unsigned int virtualAddress)
{
	int tableOffset = mmuGetTableIndex(virtualAddress, INDEX_OF_L2_PAGE_TABLE, TTBR0);

	if(tableOffset < VALID_PAGE_TABLE_OFFSET)
	{
		// no existing L2 page table, create one
		return mmuCreatePageTable(L2_PAGE_TABLE);
	}
	else
	{
		// L2 page table exists
		pageTablePointer_t l2PageTable = (pageTableL1 + tableOffset);
		return l2PageTable;
	}
}


/**
 * \brief	Gets the L1, L2 or page frame index out of a virtual address. see p.B3-1335
 */
static uint32_t mmuGetTableIndex(unsigned int virtualAddress, unsigned int indexType, unsigned int ttbrType)
{
	uint32_t upperBitsMask = 0;

	if(TTBR1 == ttbrType)
	{
		upperBitsMask = L1_PAGE_TABLE_INDEX_NATIVE_MASK;
	}
	else
	{
		upperBitsMask = L1_PAGE_TABLE_INDEX_MASK;
	}

	switch(indexType)
	{
		case INDEX_OF_L1_PAGE_TABLE:
			return ((virtualAddress & upperBitsMask) >> L1_INDEX_POSITION_IN_VIRTUAL_ADDRESS);
		case INDEX_OF_L2_PAGE_TABLE:
			return ((virtualAddress & L2_PAGE_TABLE_INDEX_MASK) >> L2_INDEX_POSITION_IN_VIRTUAL_ADDRESS);
		case INDEX_OF_PAGE_FRAME:
			return (virtualAddress & PAGE_FRAME_INDEX_MASK);
		default:
			return -1;
	}
}
