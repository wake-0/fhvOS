/*
 * memmanager.h
 *
 *  Created on: 04.04.2015
 *      Author: Marko
 */

#ifndef MEMMANAGER_MEMMANAGER_H_
#define MEMMANAGER_MEMMANAGER_H_

#include <stdlib.h>

#define PAGE_SIZE_64KB						0x10000
#define PAGE_SIZE_4KB						0x1000
#define PAGE_SIZE							PAGE_SIZE_64KB
#define MEMORY_REGIONS						5

#define BOOT_ROM_START_ADDRESS				0x40000000
#define BOOT_ROM_END_ADDRESS				0x4002BFFF
#define MEMORY_MAPPED_IO_START_ADDRESS 		0x40300000
#define MEMORY_MAPPED_IO_END_ADDRESS 		0x7FFFFFFF
#define SDRAM_START_ADDRESS					0x80000000
#define SDRAM_END_ADDRESS					0xBFFFFFFF
#define INTERNAL_SRAM_START_ADDRESS			0x402F0400
#define INTERNAL_SRAM_END_ADDRESS			0x402FFFFF
#define KERNEL_START_ADDRESS				0x80000000
#define KERNEL_END_ADDRESS					0x80FFFFFF
#define PROCESS_PAGES_START_ADDRESS			0x81000000
#define PROCESS_PAGES_END_ADDRESS			0xBFFFFFFF

typedef enum { false, true } bool_t;

typedef struct
{
	bool_t reserved;
	unsigned int processID;
} pageStatus_t;


// Data type for organizing the memory sections given in the ARM TRM.

typedef struct region
{
	bool_t directAccess;
    unsigned int startAddress;
    unsigned int endAddress;
    unsigned int length;
    unsigned int numberOfPages;
    unsigned int reservedPages;
    pageStatus_t const * pageStatusLookup;
} memorySection_t;


extern void MemoryManagerInit();

#endif /* MEMMANAGER_MEMMANAGER_H_ */
