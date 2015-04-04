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


static void MemoryManagerInitializeSection(memorySectionPointer_t, boolean_t, unsigned int, unsigned int );
static boolean_t MemoryManagerSufficientSpace(memorySectionPointer_t section, unsigned int pagesToReserve);

memorySection_t memorySections[MEMORY_REGIONS];


/**
 * \brief	This function initializes the existing memory sections.
 * 			See Memory Mapping in ARM Technical Reference Manual.
 * \param  	None
 * \return 	true if successfully initialized
 */
int MemoryManagerInit()
{
	MemoryManagerInitializeSection(&memorySections[0], true, BOOT_ROM_START_ADDRESS, BOOT_ROM_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[1], true, INTERNAL_SRAM_START_ADDRESS, INTERNAL_SRAM_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[2], true, MEMORY_MAPPED_IO_START_ADDRESS, MEMORY_MAPPED_IO_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[3], true, KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[4], false, PROCESS_PAGES_START_ADDRESS, PROCESS_PAGES_END_ADDRESS);
	return MEMORY_OK;
}


/**
 * \brief	This function initializes a memory section. It calculates the number of maximum
 * 			available pages and reserves memory for the page satus structs.
 * \param  	access				- defines if accessed directly or over virtual memory management
 * \return 	None
 */
static void MemoryManagerInitializeSection(memorySectionPointer_t memorySection, boolean_t access, unsigned int startAddress, unsigned int endAddress)
{
	memorySection->startAddress 	= startAddress;
	memorySection->endAddress 		= endAddress;
	memorySection->length 			= endAddress - startAddress;
	memorySection->directAccess 	= access;
	memorySection->numberOfPages 	= (memorySection->length / PAGE_SIZE);
	memorySection->reservedPages 	= 0;
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
	return MEMORY_OK;
}


static boolean_t MemoryManagerSufficientSpace(memorySectionPointer_t section, unsigned int pagesToReserve)
{
	unsigned int freePagesInSection = (section->numberOfPages - section->reservedPages);

	if(freePagesInSection > pagesToReserve)
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

	if(false == MemoryManagerSufficientSpace(section, pagesToReserve))
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


void MemoryManagerGetFreePagesInSection()
{

}

