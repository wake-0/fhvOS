// MMU Functions
extern int MMUInit(void);
extern int MMUSwitchToProcess(process_t* process);
extern int MMUInitProcess(process_t* process);
extern void MMUHandleDataAbortException(context_t* context);
extern int MMUFreeAllPageFramesOfProcess(process_t* process);

// MemoryManager Functions
extern int MemoryManagerInit();
extern pageAddressPointer_t MemoryManagerGetFreePagesInProcessRegion(unsigned int pagesToReserve);
extern memoryRegionPointer_t MemoryManagerGetRegion(unsigned int memoryRegionNumber);
extern int MemoryManagerReserveAllDirectMappedRegions(void);