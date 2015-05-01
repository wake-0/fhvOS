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


#define MMU_OK 		0x1;
#define MMU_NOT_OK 	0x0;

typedef uint32_t* pageTablePointer_t;

extern int MMUInit(void);
extern int MMUSwitchToProcess(process_t* process);
extern int MMUInitProcess(process_t* process);
extern void MMUHandleDataAbortException();



#endif /* MEMMANAGER_MMU_H_ */
