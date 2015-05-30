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
#define PROCESS_MANAGER_MAX_PROCESS_LENGTH		(15)

typedef struct
{
	char processName[PROCESS_MANAGER_MAX_PROCESS_LENGTH];
	long startTime;
	int processID;
	int state;
} processInfoAPI_t;

extern void ProcessManagerInit(void);
extern process_t* ProcessManagerStartProcess(char * processName, void (*funcPtr)(int, char ** ), int, char**, boolean_t, context_t*);
extern void ProcessManagerKillProcess(processId_t processId);
extern int ProcessManagerGetRunningProcessesCount(void);
extern void ProcessManagerListProcesses(processInfoAPI_t* processAPIPtr, int length);

#endif /* PROCESSMANAGER_PROCESSMANAGER_H_ */
