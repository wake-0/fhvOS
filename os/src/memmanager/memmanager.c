/*
 * memmanager.c
 *
 *	This file contains the functions for the virtual memory management.
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


#include "memmanager.h"
#include <stdlib.h>


static void memoryManagerInitializeRegion(memoryRegionPointer_t, boolean_t, unsigned int, unsigned int );
static boolean_t memoryManagerSufficientSpace(memoryRegionPointer_t region, unsigned int pagesToReserve);
static pageAddressPointer_t memoryManagerGetPageAddress(memoryRegionPointer_t region, unsigned int pageNumber);
static int memoryManagerLookupSectionForFreePagesInRow(memoryRegionPointer_t region, unsigned int startingPageNumber, unsigned int pagesToReserve);
static void memoryManagerReservePagesInARow(memoryRegionPointer_t region, unsigned int pageNumber, unsigned int pagesToReserve);
static void memoryManagerReserveDirectMappedRegion(unsigned int memoryRegion);

memoryRegion_t memorySections[MEMORY_REGIONS];


/**
 * \brief	This function initializes the existing memory sections.
 * 			See Memory Mapping in ARM Technical Reference Manual.
 * \param  	None
 * \return 	true if successfully initialized
 */
int MemoryManagerInit()
{
	memoryManagerInitializeRegion(&memorySections[BOOT_ROM_REGION], true, BOOT_ROM_START_ADDRESS, BOOT_ROM_END_ADDRESS);
	memorySections[BOOT_ROM_REGION].pageStatus = &romRegion[0];

	memoryManagerInitializeRegion(&memorySections[MEMORY_MAPPED_IO_REGION], true, MEMORY_MAPPED_IO_START_ADDRESS, MEMORY_MAPPED_IO_END_ADDRESS);
	memorySections[MEMORY_MAPPED_IO_REGION].pageStatus = &mmioRegion[0];

	memoryManagerInitializeRegion(&memorySections[KERNEL_REGION], true, KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	memorySections[KERNEL_REGION].pageStatus = &kernelRegion[0];

	memoryManagerInitializeRegion(&memorySections[PAGE_TABLE_REGION], true, PAGE_TABLES_START_ADDRESS, PAGE_TABLES_END_ADDRESS);
	memorySections[PAGE_TABLE_REGION].pageStatus = &tableRegion[0];

	memoryManagerInitializeRegion(&memorySections[PROCESS_REGION], false, PROCESS_PAGES_START_ADDRESS, PROCESS_PAGES_END_ADDRESS);
	memorySections[PROCESS_REGION].pageStatus = &procRegion[0];

	return MEMORY_OK;
}


/**
 * \brief	This function initializes a memory region. It calculates the number of maximum
 * 			available pages and reserves memory for the page satus structs.
 * \param  	access				- defines if accessed directly or over virtual memory management
 * \return 	None
 */
static void memoryManagerInitializeRegion(memoryRegionPointer_t memoryRegion, boolean_t access, unsigned int startAddress, unsigned int endAddress)
{
	memoryRegion->startAddress 		= startAddress;
	memoryRegion->endAddress 		= endAddress;
	//memoryRegion->directAccess 		= access;
	int length 						= endAddress - startAddress;
	memoryRegion->numberOfPages 	= (length / PAGE_SIZE_4KB);
	memoryRegion->reservedPages 	= 0;
	//memoryRegion->pageStatus	 	= (pageStatusPointer_t)malloc( sizeof(pageStatus_t) * memoryRegion->numberOfPages);
	//memset(memoryRegion->pageStatus,0, sizeof(pageStatus_t) * memoryRegion->numberOfPages);
	// TODO Handle case (pageStatus == 0)
}


memoryRegionPointer_t MemoryManagerGetRegion(unsigned int memorySectionNumber)
{
	return &memorySections[memorySectionNumber];
}


/**
 * \brief	Reserves a single page.
 * 			Set reserved status of page to true.
 *			Increase number of reserved pages of the region.
 * \return 	True if reservation was successfull
 */
int MemoryManagerReserveSinglePage(memoryRegionPointer_t region, unsigned int pageNumber)
{
	if(pageNumber > region->numberOfPages)
	{
		return MEMORY_NOT_OK;
	}

	region->pageStatus[pageNumber].reserved = true;
	region->reservedPages++;
	return MEMORY_OK;
}


/**
 * \brief	Evaluates if enough free pages are left in a memory region.
 * \return 	True if enough space left, else false.
 */
static boolean_t memoryManagerSufficientSpace(memoryRegionPointer_t region, unsigned int pagesToReserve)
{
	int unreservedPages = region->numberOfPages - region->reservedPages;
	if(unreservedPages > pagesToReserve)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * \brief	Reserves a a number of pages if enough space is available.
 * \return 	True if reservation was successfull
 */
int MemoryManagerReserveMultiplePages(unsigned int memoryRegion, unsigned int pagesToReserve)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(memoryRegion);
	unsigned int reservedPages = 0;

	if(false == memoryManagerSufficientSpace(region, pagesToReserve))
	{
		return MEMORY_NOT_OK;
	}

	while(reservedPages < pagesToReserve)
	{
		MemoryManagerReserveSinglePage(region, reservedPages);
		reservedPages++;
	}

	return MEMORY_OK;
}


/**
 * \brief	Reserves all pages of a memory region.
 * \return 	none
 */
void MemoryManagerReserveAllPages(memoryRegionPointer_t region)
{
	unsigned int page;

	for(page = 0; page < region->numberOfPages; page++)
	{
		MemoryManagerReserveSinglePage(region, page);
	}
}


/**
 * \brief	Finds a number of pages of a specified region in a row.
 * \return	Address of first page in row if successfull, otherwise null.
 */
pageAddressPointer_t MemoryManagerGetFreePagesInRegion(unsigned int memoryRegion, unsigned int pagesToReserve)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(memoryRegion);
	int unreservedPages = region->numberOfPages - region->reservedPages;

	if((pagesToReserve > region->numberOfPages) || (pagesToReserve > unreservedPages) || (pagesToReserve == 0))
	{
		return NULL;
	}

	unsigned int pageNumber;
	int pagesLookupStatus = 0;

	// look up all pages of specified region
	for(pageNumber = 0; pageNumber <= region->numberOfPages; pageNumber++)
	{
		if((pageNumber + pagesToReserve) > region->numberOfPages)
		{
			return NULL;
		}

		if(region->pageStatus[pageNumber].reserved)
		{
			continue;
		}
		else
		{
			pagesLookupStatus = memoryManagerLookupSectionForFreePagesInRow(region, pageNumber, pagesToReserve);
		}

		if(PAGES_IN_A_ROW_FOUND == pagesLookupStatus)
		{
			break;
		}
		else
		{
			pageNumber += pagesLookupStatus;
			pagesLookupStatus = -1;
		}
	}

	memoryManagerReservePagesInARow(region, pageNumber, pagesToReserve);

	return memoryManagerGetPageAddress(region, pageNumber);
}


/**
 * \brief	Reserves pagesToReserve count pages of a region in a row.
 * \return	none
 */
static void memoryManagerReservePagesInARow(memoryRegionPointer_t region, unsigned int pageNumber, unsigned int pagesToReserve)
{
	int i;
	for(i = 0; i < pagesToReserve; i++)
	{
		region->pageStatus[pageNumber+i].reserved = true;
	}
}


/**
 * \brief	Reserves a table of pagesToReserve count pages of pageSize size.
 * \return	Starting ddress of table if successfull, otherwise null.
 */
pageAddressPointer_t MemoryManagerCreatePageTable(unsigned int pageTableType)
{
	pageAddressPointer_t tableStartAddress = 0;
	int pagesToReserve = 0;

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

	tableStartAddress = MemoryManagerGetFreePagesInRegion(PAGE_TABLE_REGION, pagesToReserve);

	if(NULL == tableStartAddress)
	{
		return NULL;
	}

	memset(tableStartAddress, FAULT_PAGE_TABLE_ENTRY, PAGE_SIZE_4KB * pagesToReserve);
	return tableStartAddress;
}


/**
 * \brief	Evaluates if a certain number of free pages are in a row in a memory region.
 * \return 	Returns 0 if enough free pages in a row, counted pages else.
 */
static int memoryManagerLookupSectionForFreePagesInRow(memoryRegionPointer_t region, unsigned int startingPageNumber, unsigned int pagesToReserve)
{
	int i = 0;

	for(i = startingPageNumber; i < (startingPageNumber + pagesToReserve); i++)
	{
		if(true == region->pageStatus[i].reserved)
		{
			return i;
		}
	}

	return 0;
}


/**
 * \brief	Gets the address of a page if page is existing.
 * \return	Pointer on Address of Page
 */
static pageAddressPointer_t memoryManagerGetPageAddress(memoryRegionPointer_t region, unsigned int pageNumber)
{
	if(pageNumber > region->numberOfPages)
	{
		return NULL;
	}

	pageAddressPointer_t pageAddress = (pageAddressPointer_t)(region->startAddress + pageNumber * PAGE_SIZE_4KB);
	return pageAddress;
}


/**
 * \brief	Reserves all pages of all regions except page table and process region.
 */
int MemoryManagerReserveAllDirectMappedRegions(void)
{
	unsigned int memoryRegion;

	for(memoryRegion = 0; memoryRegion < MEMORY_REGIONS - 2; memoryRegion++)
	{
		memoryManagerReserveDirectMappedRegion(memoryRegion);
	}

	return MEMORY_OK;
}

static void memoryManagerReserveDirectMappedRegion(unsigned int memoryRegion)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(memoryRegion);

	//if(TRUE == region->directAccess)
	{
		MemoryManagerReserveAllPages(region);
	}
}
