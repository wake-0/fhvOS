/*
 * coprocessor.h
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


extern void MMUEnable(void);
extern void MMUDisable(void);
extern void MMUFlushTLB(void);
extern void MMUSetProcessTable(unsigned int processTable);
extern void MMUSeMMUSetKernelTabletKernelTable(unsigned int kernelTable);
extern void MMUSetDomainAccess(unsigned int domainAccessPermission);
extern void MMULoadDabtData(void);
extern void InstructionCacheEnable();
extern void InstructionCacheDisable();
extern void InstructionCacheFlush();
extern void DataCacheEnable();
