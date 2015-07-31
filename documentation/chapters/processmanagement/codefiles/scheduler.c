#pragma pack(push,1)
typedef struct {
	address_t* cpsr;
	address_t* lr;
	address_t* sp;
	register_t registers[REGISTER_COUNT];
	address_t* pc;
} context_t;
#pragma pack(pop)

struct process_t_struct {
	processId_t id;
	processFunc func;
	processState_t state;

	context_t* context;
	address_t* pageTableL1;

	long wakeupTime;

	[...]
};

/*
 * Scheduler functions
 */
extern int SchedulerInit(device_t stdoutDevice);
extern int SchedulerStart(device_t timerDevice);
extern process_t* SchedulerStartProcess(processFunc func);
extern int SchedulerRunNextProcess(context_t* context);
extern int SchedulerKillProcess(processId_t id);
extern process_t* SchedulerGetRunningProcess(void);
extern void SchedulerBlockProcess(processId_t process);
extern void SchedulerUnblockProcess(processId_t process);
extern void SchedulerSleepProcess(processId_t process, unsigned int millis);
extern void SchedulerDisableScheduling(void);
extern void SchedulerEnableScheduling(void);