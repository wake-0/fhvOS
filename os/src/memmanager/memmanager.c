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


static void memoryManagerInitializeRegion(memoryRegionPointer_t, unsigned int, unsigned int );
static pageAddressPointer_t memoryManagerGetPageAddress(memoryRegionPointer_t region, unsigned int pageNumber);
static int memoryManagerLookupSectionForFreePagesInRow(memoryRegionPointer_t region, unsigned int startingPageNumber, unsigned int pagesToReserve);
static void memoryManagerReservePagesInARow(memoryRegionPointer_t region, unsigned int pageNumber, unsigned int pagesToReserve);
static void memoryManagerReserveDirectMappedRegion(unsigned int memoryRegion);
static int memoryManagerReserveSinglePage(memoryRegionPointer_t region, unsigned int pageNumber);
static void memoryManagerReserveAllPagesOfRegion(memoryRegionPointer_t region);

// contains the regions of the virtual memory
static memoryRegion_t memorySections[MEMORY_REGIONS];


/**
 * \brief	This function initializes the existing memory sections.
 * 			See Memory Mapping in ARM Technical Reference Manual.
 * \param  	None
 * \return 	true if successfully initialized
 */
int MemoryManagerInit()
{
	memoryManagerInitializeRegion(&memorySections[BOOT_ROM_REGION], BOOT_ROM_START_ADDRESS, BOOT_ROM_END_ADDRESS);
	memorySections[BOOT_ROM_REGION].pageStatus = &romRegion[0];

	memoryManagerInitializeRegion(&memorySections[MEMORY_MAPPED_IO_REGION], MEMORY_MAPPED_IO_START_ADDRESS, MEMORY_MAPPED_IO_END_ADDRESS);
	memorySections[MEMORY_MAPPED_IO_REGION].pageStatus = &mmioRegion[0];

	memoryManagerInitializeRegion(&memorySections[KERNEL_REGION], KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	memorySections[KERNEL_REGION].pageStatus = &kernelRegion[0];

	memoryManagerInitializeRegion(&memorySections[PAGE_TABLE_REGION], PAGE_TABLES_START_ADDRESS, PAGE_TABLES_END_ADDRESS);
	memorySections[PAGE_TABLE_REGION].pageStatus = &tableRegion[0];

	memoryManagerInitializeRegion(&memorySections[PROCESS_REGION], PROCESS_PAGES_START_ADDRESS, PROCESS_PAGES_END_ADDRESS);
	memorySections[PROCESS_REGION].pageStatus = &procRegion[0];
	procRegion->reserved = TRUE;

	memoryManagerInitializeRegion(&memorySections[INTERNAL_SRAM_REGION], INTERNAL_SRAM_START_ADDRESS, INTERNAL_SRAM_END_ADDRESS);
	memorySections[INTERNAL_SRAM_REGION].pageStatus = &sramRegion[0];

	memoryManagerInitializeRegion(&memorySections[BOOT_ROM_EXCEPTIONS_REGION], BOOT_ROM_EXCEPTIONS_START_ADDRESS, BOOT_ROM_EXCEPTIONS_END_ADDRESS);
	memorySections[BOOT_ROM_EXCEPTIONS_REGION].pageStatus = &romExceptionsRegion[0];

	// comment in when using hivecs
	//memoryManagerInitializeRegion(&memorySections[HIVECS_REGION], HIVECS_START_ADDRESS, HIVECS_END_ADDRESS);
	//memorySections[HIVECS_REGION].pageStatus = &hivecsRegion[0];

	return MEMORY_OK;
}


/**
 * \brief	This function initializes a memory region. It calculates the number of maximum
 * 			available pages and reserves memory for the page satus structs.
 * \return 	None
 */
static void memoryManagerInitializeRegion(memoryRegionPointer_t memoryRegion, unsigned int startAddress, unsigned int endAddress)
{
	memoryRegion->startAddress 		= startAddress;
	memoryRegion->endAddress 		= endAddress;
	int length 						= endAddress - startAddress;

	if((length / PAGE_SIZE_4KB) < 0)
	{
		memoryRegion->numberOfPages 	= 1;
	}
	else
	{
		memoryRegion->numberOfPages 	= (length / PAGE_SIZE_4KB);
	}

	memoryRegion->reservedPages 	= 0;
	// TODO Handle case (pageStatus == 0)
}


/**
 * \brief	Evaluates if enough free pages are left in a memory region.
 * \return 	Pointer on the memory region if successful, null else
 */
memoryRegionPointer_t MemoryManagerGetRegion(unsigned int memoryRegionNumber)
{
	if(memoryRegionNumber < MEMORY_REGIONS)
	{
		return &memorySections[memoryRegionNumber];
	}
	else
	{
		return NULL;
	}
}


/**
 * \brief	Reserves a single page.
 * 			Set reserved status of page to true.
 *			Increase number of reserved pages of the region.
 * \return 	True if reservation was successfull
 */
static int memoryManagerReserveSinglePage(memoryRegionPointer_t region, unsigned int pageNumber)
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
 * \brief	Finds a number of pages of a specified region in a row.
 * \return	Address of first page in row if successfull, otherwise null.
 */
pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(PROCESS_REGION);
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
 * 			This is done in order to prevent accidently allocation of pages in non-process space regions.
 */
int MemoryManagerReserveAllDirectMappedRegions(void)
{
	memoryManagerReserveDirectMappedRegion(BOOT_ROM_REGION);
	memoryManagerReserveDirectMappedRegion(MEMORY_MAPPED_IO_REGION);
	memoryManagerReserveDirectMappedRegion(BOOT_ROM_EXCEPTIONS_REGION);
	memoryManagerReserveDirectMappedRegion(INTERNAL_SRAM_REGION);
	memoryManagerReserveDirectMappedRegion(KERNEL_REGION);
	return MEMORY_OK;
}

static void memoryManagerReserveDirectMappedRegion(unsigned int memoryRegion)
{
	memoryRegionPointer_t region = MemoryManagerGetRegion(memoryRegion);
	memoryManagerReserveAllPagesOfRegion(region);
}

/**
 * \brief	Reserves all pages of a memory region.
 * \return 	none
 */
static void memoryManagerReserveAllPagesOfRegion(memoryRegionPointer_t region)
{
	unsigned int page;

	for(page = 0; page < region->numberOfPages; page++)
	{
		memoryManagerReserveSinglePage(region, page);
	}
}
