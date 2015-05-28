/*
 * processmanager.h
 *
 *  Created on: 14.05.2015
 *      Author: Marko
 */

#ifndef PROCESSMANAGER_PROCESSMANAGER_H_
#define PROCESSMANAGER_PROCESSMANAGER_H_

#include "../scheduler/scheduler.h"

#define PROCESSMANAGER_FAILURE			-1

typedef struct
{
	char * processName;
	long startTime;
	process_t* processScheduler;
} processInfo_t;

// return value of processList function
typedef struct
{
	char * processName;
	long startTime;
	int processID;
	processState_t state;
} processInfoAPI_t;

extern void ProcessManagerInit(void);
extern process_t* ProcessManagerStartProcess(char * processName, void (*funcPtr)(int, char ** ), int, char**, boolean_t, context_t*);
extern void ProcessManagerKillProcess(int processId);
extern int ProcessManagerGetRunningProcessesCount(void);
extern void ProcessManagerListProcesses(processInfoAPI_t* processAPIPtr, int length);

#endif /* PROCESSMANAGER_PROCESSMANAGER_H_ */
