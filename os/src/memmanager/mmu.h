/*
 * mmu.h
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */

#ifndef MEMMANAGER_MMU_H_
#define MEMMANAGER_MMU_H_

#include "../hal/cpu/coprocessor.h"
#include "../scheduler/scheduler.h"
#include <stdio.h>
#include "memmanager.h"


#define MMU_OK 							0x1
#define MMU_NOT_OK 						0x0
#define DESCRIPTOR_TYPE_SECTION 		0x2
#define DESCRIPTOR_TYPE_PAGE_TABLE 		0x1
#define DESCRIPTOR_TYPE_SMALL_PAGE		0x2
#define DESCRIPTOR_TYPE_LARGE_PAGE		0x1
#define FULL_ACCESS						0x3
#define INDEX_OF_L1_PAGE_TABLE 			0x0
#define INDEX_OF_L2_PAGE_TABLE 			0x1
#define INDEX_OF_PAGE_FRAME				0x2
#define TTBR1							0x1
#define TTBR0							0x0

#define VALID_PAGE_TABLE_OFFSET			0x0
#define INVALID_PAGE_TABLE_OFFSET		0xFFFFFFFF

#define BOUNDARY_SELECTION_MASK					0x7
#define BOUNDARY_AT_HALF_OF_VIRTUAL_MEMORY		0x1
#define BOUNDARY_AT_QUARTER_OF_MEMORY			0x2		// see p. B3-1330, table B3-1
#define N										BOUNDARY_AT_QUARTER_OF_MEMORY
#define L1_PAGE_TABLE_INDEX_NATIVE_MASK 0xFFF00000
#define L1_PAGE_TABLE_INDEX_MASK (L1_PAGE_TABLE_INDEX_NATIVE_MASK >> N) & L1_PAGE_TABLE_INDEX_NATIVE_MASK
#define L2_PAGE_TABLE_INDEX_MASK 	0xFF000
#define PAGE_FRAME_INDEX_MASK		0xFFF

typedef uint32_t* pageTablePointer_t;

typedef struct
{
	unsigned int sectionBaseAddress;
	unsigned int accessPermission : 2;
	unsigned int domain : 4;
	unsigned int cachedBuffered : 2;
	unsigned int descriptorType : 2;

} firstLevelDescriptor_t;


typedef struct
{
	unsigned int pageBaseAddress;
	unsigned int accessPermission : 2;
	unsigned int TEX : 3;
	unsigned int cachedBuffered : 2;
	unsigned int descriptorType : 2;
} secondLevelDescriptor_t;


extern int MMUInit(void);
extern int MMUSwitchToProcess(process_t* process);
extern int MMUInitProcess(process_t* process);
extern void MMUHandleDataAbortException(context_t* context);
extern int MMUFreeAllPageFramesOfProcess(process_t* process);

#endif /* MEMMANAGER_MMU_H_ */
