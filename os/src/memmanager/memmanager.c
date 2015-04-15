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


static void memoryManagerInitializeSection(memorySectionPointer_t, boolean_t, unsigned int, unsigned int );
static boolean_t memoryManagerSufficientSpace(memorySectionPointer_t section, unsigned int pagesToReserve);
static pageAddressPointer_t memoryManagerGetPageAddress(memorySectionPointer_t section, unsigned int pageNumber);
static int memoryManagerLookupSectionForFreePagesInRow(memorySectionPointer_t section, unsigned int startingPageNumber, unsigned int pagesToReserve);

memorySection_t memorySections[MEMORY_REGIONS];


/**
 * \brief	This function initializes the existing memory sections.
 * 			See Memory Mapping in ARM Technical Reference Manual.
 * \param  	None
 * \return 	true if successfully initialized
 */
int MemoryManagerInit()
{
	memoryManagerInitializeSection(&memorySections[0], true, BOOT_ROM_START_ADDRESS, BOOT_ROM_END_ADDRESS);
	memoryManagerInitializeSection(&memorySections[1], true, INTERNAL_SRAM_START_ADDRESS, INTERNAL_SRAM_END_ADDRESS);
	memoryManagerInitializeSection(&memorySections[2], true, MEMORY_MAPPED_IO_START_ADDRESS, MEMORY_MAPPED_IO_END_ADDRESS);
	memoryManagerInitializeSection(&memorySections[3], true, KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	memoryManagerInitializeSection(&memorySections[4], false, PROCESS_PAGES_START_ADDRESS, PROCESS_PAGES_END_ADDRESS);
	return MEMORY_OK;
}


/**
 * \brief	This function initializes a memory section. It calculates the number of maximum
 * 			available pages and reserves memory for the page satus structs.
 * \param  	access				- defines if accessed directly or over virtual memory management
 * \return 	None
 */
static void memoryManagerInitializeSection(memorySectionPointer_t memorySection, boolean_t access, unsigned int startAddress, unsigned int endAddress)
{
	memorySection->startAddress 	= startAddress;
	memorySection->endAddress 		= endAddress;
	memorySection->length 			= endAddress - startAddress;
	memorySection->directAccess 	= access;
	memorySection->numberOfPages 	= (memorySection->length / PAGE_SIZE);
	memorySection->reservedPages 	= 0;
	memorySection->unreservedPages	= memorySection->numberOfPages;
	memorySection->pageStatus	 	= (pageStatusPointer_t)malloc( sizeof(pageStatus_t) * memorySection->length );
}


memorySectionPointer_t MemoryManagerGetSection(unsigned int memorySectionNumber)
{
	return &memorySections[memorySectionNumber];
}


/**
 * \brief	Reserves a single page.
 * 			Set reserved status of page to true.
 *			Increase number of reserved pages of the section.
 * \return 	True if reservation was successfull
 */
int MemoryManagerReserveSinglePage(memorySectionPointer_t section, unsigned int pageNumber)
{
	section->pageStatus[pageNumber].reserved = true;
	section->reservedPages++;
	section->unreservedPages--;
	return MEMORY_OK;
}


static boolean_t memoryManagerSufficientSpace(memorySectionPointer_t section, unsigned int pagesToReserve)
{
	if(section->unreservedPages > pagesToReserve)
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
int MemoryManagerReserveMultiplePages(unsigned int memorySectionNumber, unsigned int pagesToReserve)
{
	memorySectionPointer_t section = MemoryManagerGetSection(memorySectionNumber);
	unsigned int reservedPages = 0;

	if(false == memoryManagerSufficientSpace(section, pagesToReserve))
	{
		return MEMORY_NOT_OK;
	}

	while(reservedPages < pagesToReserve)
	{
		MemoryManagerReserveSinglePage(section, reservedPages);
		reservedPages++;
	}

	return MEMORY_OK;
}


/**
 * \brief	Finds a number of pages of a specified section in a row.
 * \return	Address of first page in row if successfull, otherwise null.
 */
pageAddressPointer_t MemoryManagerGetFreePagesInSection(unsigned int memorySection, unsigned int pagesToReserve)
{
	memorySectionPointer_t section = MemoryManagerGetSection(memorySection);

	if((pagesToReserve > section->numberOfPages) || (pagesToReserve > section->unreservedPages) || (pagesToReserve == 0))
	{
		return NULL;
	}

	unsigned int pageNumber;
	int pagesLookupStatus = 0;

	// look up all pages of specified section
	for(pageNumber = 0; pageNumber <= section->numberOfPages; pageNumber++)
	{
		if((pageNumber + pagesToReserve) > section->numberOfPages)
		{
			return NULL;
		}

		if(section->pageStatus[pageNumber].reserved)
		{
			continue;
		}
		else
		{
			pagesLookupStatus = memoryManagerLookupSectionForFreePagesInRow(section, pageNumber, pagesToReserve);
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

	return memoryManagerGetPageAddress(section, pageNumber);
}


static int memoryManagerLookupSectionForFreePagesInRow(memorySectionPointer_t section, unsigned int startingPageNumber, unsigned int pagesToReserve)
{
	int i = 0;

	for(i = startingPageNumber; i < (startingPageNumber + pagesToReserve); i++)
	{
		if(true == section->pageStatus[i].reserved)
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
static pageAddressPointer_t memoryManagerGetPageAddress(memorySectionPointer_t section, unsigned int pageNumber)
{
	if(pageNumber > section->numberOfPages)
	{
		return NULL;
	}

	pageAddressPointer_t pageAddress = (pageAddressPointer_t)(section->startAddress + pageNumber * PAGE_SIZE);
	return pageAddress;
}

