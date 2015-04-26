/*
 * scheduler.h
 *
 *  Created on: 01.04.2015
 *      Author: Kevin
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

#include "../platform/platform.h"

/*
 * Defines for better understanding
 */
#define SCHEDULER_OK		(0)
#define SCHEDULER_ERROR		(-1)

#define PROCESSES_MAX		(64)
#define REGISTER_COUNT		(13)

#define INVALID_PROCESS_ID	(-1)

/*
 * Typedefs for the process struct
 */
typedef int8_t processId_t;
typedef void (*processFunc)(void);

typedef address_t* register_t;

/*
 * Enums for the process struct
 */
typedef enum {
	READY,
	RUNNING,
	BLOCKED,
	FREE
} processState_t;

// Process switch context
#pragma pack(push,1)
typedef struct {
	address_t* cpsr;
	address_t* lr;
	//address_t* sp;
	register_t registers[REGISTER_COUNT];
	register_t* pc;
} context_t;
#pragma pack(pop)

typedef struct {
	processId_t id;
	processFunc func;
	processState_t state;

	context_t* context;
} process_t;

/*
 * Scheduler functions
 */
extern int SchedulerInit(void);
extern int SchedulerStartProcess(processFunc func);
extern int SchedulerRunNextProcess(context_t* context);
extern int SchedulerKillProcess(processId_t id);
extern process_t* SchedulerGetRunningProcess(void);

#endif /* SCHEDULER_SCHEDULER_H_ */
