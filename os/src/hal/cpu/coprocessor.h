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
extern void MMUSetKernelTable(unsigned int kernelTable);
extern void MMUSetDomainAccess(unsigned int domainAccessPermission);
extern void MMULoadDabtData(void);
extern void InstructionCacheEnable(void);
extern void InstructionCacheDisable(void);
extern void InstructionCacheFlush(void);
extern void DataCacheEnable(void);
extern void MMUSetTranslationTableControlRegister(unsigned int translationTableControlRegisterSettings);
extern void MMUReadProcessTableAddress(void);
extern void MMUReadKernelTableAddress(void);
extern void MMUReadSystemControlRegister(void);
extern void MMUReadTTBCR(void);
extern void MMUChangeTTBR0andContextId(unsigned int baseAddressTTBR0, unsigned int contextId);
extern void MMUReadContextIdRegister(void);
extern void CleanDataCache(void);
