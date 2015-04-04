/*
 * coprocessor.h
 *
 *  Created on: 04.04.2015
 *      Author: Marko Petrovic
 */


extern void MMUEnable();
extern void MMUDisable();
extern void MMUFlushTLB();
extern void MMUSetProcessTable();
extern void MMUSetKernelTable();
extern void MMUSetDomainAccess();
extern void MMULoadDabtData();
extern void InstructionCacheEnable();
extern void InstructionCacheDisable();
extern void InstructionCacheFlush();
extern void DataCacheEnable();
