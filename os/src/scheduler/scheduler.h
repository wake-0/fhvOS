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
#define REGISTER_COUNT		(15)

#define INVALID_PROCESS_ID	(-1)

/*
 * Typedefs for the process struct
 */
typedef int8_t processId_t;
typedef void (*processFunc)(void);

typedef void* register_t;
typedef uint32_t* pc_t;
typedef uint32_t cpsr_t;

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
typedef struct {
	register_t registers[REGISTER_COUNT];
	pc_t pc;
	cpsr_t cpsr;
} context_t;

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
