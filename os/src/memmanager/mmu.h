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
#include "memmanager.h"

#define MMU_MASTER_TABLE_PAGE_COUNT 4
#define MMU_MASTER_TABLE_SIZE MEM_PAGE_SIZE * MMU_MASTER_TABLE_PAGE_COUNT

typedef uint32_t* pageTablePointer_t;

extern void MMUInit(void);
extern void MMUSwitchToProcess(process_t process);
extern void MMUInitProcess(process_t process);
extern pageTablePointer_t MMUCreateMasterPageTable(void);



#endif /* MEMMANAGER_MMU_H_ */
