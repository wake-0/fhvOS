/*
 * memmanager.c
 *
 *	This file contains the functions for the virtual memory management.
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


#include "memmanager.h"

static void MemoryManagerInitializeSection(memorySection_t *, bool_t, unsigned int, unsigned int );

memorySection_t memorySections[MEMORY_REGIONS];


/**
 * \brief	This function initializes the existing memory sections.
 * 			See Memory Mapping in ARM Technical Reference Manual.
 * \param  	None
 * \return 	None
 */
void MemoryManagerInit()
{
	MemoryManagerInitializeSection(&memorySections[0], true, BOOT_ROM_START_ADDRESS, BOOT_ROM_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[1], true, INTERNAL_SRAM_START_ADDRESS, INTERNAL_SRAM_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[2], true, MEMORY_MAPPED_IO_START_ADDRESS, MEMORY_MAPPED_IO_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[3], true, KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	MemoryManagerInitializeSection(&memorySections[4], false, PROCESS_PAGES_START_ADDRESS, PROCESS_PAGES_END_ADDRESS);
}


/**
 * \brief	This function initializes a memory section. It calculates the number of maximum
 * 			available pages and reserves memory for the page satus structs.
 * \param  	access				- defines if accessed directly or over virtual memory management
 * \return 	None
 */
static void MemoryManagerInitializeSection(memorySection_t * memorySection, bool_t access, unsigned int startAddress, unsigned int endAddress)
{
	memorySection->startAddress 	= startAddress;
	memorySection->endAddress 		= endAddress;
	memorySection->length 			= endAddress - startAddress;
	memorySection->directAccess 	= access;
	memorySection->numberOfPages 	= (memorySection->length / PAGE_SIZE);
	memorySection->reservedPages 	= 0;
	memorySection->pageStatusLookup = (pageStatus_t *)malloc( sizeof(pageStatus_t) * memorySection->length );
}


void MemoryManagerGetFreePagesInSection()
{

}

void MemoryManagerReservePage()
{

}
