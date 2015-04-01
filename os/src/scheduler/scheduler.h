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
#define SCHEDULER_OK		0
#define SCHEDULER_ERROR		-1
#define PROCESSES_MAX		64

/*
 * Typedefs
 */
typedef uint16_t processId_t;
typedef void (*processFunc)();

/*
 * Enums
 */
typedef enum {
	READY,
	RUNNING,
	BLOCKED
} processState_t;

typedef struct {
	processId_t id;
	processFunc func;
	processState_t state;
} process_t;

/*
 * Scheduler functions
 */
extern int SchedulerStartProcess(processFunc func);
extern int SchedulerRunNextProcess();
extern int SchedulerKillProcess(processId_t id);

#endif /* SCHEDULER_SCHEDULER_H_ */
