extern int MMUInit(void);
extern int MMUSwitchToProcess(process_t* process);
extern int MMUInitProcess(process_t* process);
extern void MMUHandleDataAbortException(void);
extern int MMUFreeAllPageFramesOfProcess(process_t* process);