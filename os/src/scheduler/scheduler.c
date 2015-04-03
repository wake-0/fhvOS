/*
 * scheduler.c
 *
 *  Created on: 01.04.2015
 *      Author: Kevin
 */
#include "scheduler.h"

/*
 * Global variables
 */
static processId_t runningProcess;
static process_t processes[PROCESSES_MAX];

/*
 * Internal functions
 */
static processId_t getNextProcessIdByState(processState_t state);
static processId_t getNextFreeProcessId(void);
static processId_t getNextReadyProcessId(void);

// Functions to stop and start the interrupts
static void atomicStart(void);
static void atomicEnd(void);

/*
 * Functions from the .h file
 */
int SchedulerInit(void) {
	int i;
	for (i = 0; i < PROCESSES_MAX; i++) {
		processes[i].state = FINISHED;
	}

	// No process is running
	runningProcess = INVALID_PROCESS_ID;
	return SCHEDULER_OK;
}

int SchedulerStartProcess(processFunc func) {
	atomicStart();

	processId_t freeProcess = getNextFreeProcessId();
	if (freeProcess == INVALID_PROCESS_ID) {
		atomicEnd();
		return SCHEDULER_ERROR;
	}

	processes[freeProcess].func = func;
	processes[freeProcess].id = freeProcess;
	processes[freeProcess].state = READY;

	atomicEnd();
	return SCHEDULER_OK;
}

int SchedulerRunNextProcess() {
	atomicStart();

	processId_t nextProcess = getNextReadyProcessId();
	if (nextProcess == INVALID_PROCESS_ID) {
		atomicEnd();
		return SCHEDULER_ERROR;
	}

	// Set the running process to ready
	processes[runningProcess].state = READY;

	// Set the new thread to running
	runningProcess = nextProcess;
	processes[runningProcess].state = RUNNING;

	atomicEnd();
	return SCHEDULER_OK;
}

int SchedulerKillProcess(processId_t id) {
	atomicStart();
	if (id < 0 || id >= PROCESSES_MAX) {
		atomicEnd();
		return SCHEDULER_ERROR;
	}

	// TODO: check the process to kill is running, when running change and then kill
	processes[id].state = FINISHED;
	processes[id].func = NULL;

	atomicEnd();
	return SCHEDULER_OK;
}

*process_t SchedulerGetRunningProcess(void) {
	if (runningProcess == INVALID_PROCESS_ID) {
		// TODO: think about this error
		// This error should normally not appear
	}

	return &processes[runningProcess];
}

/*
 * Helper methods
 */
processId_t getNextFreeProcessId(void) {
	return getNextProcessIdByState(FINISHED);
}

processId_t getNextReadyProcessId(void) {
	int i;
	for (i = (runningProcess + 1); i < PROCESSES_MAX; i++) {
		if (processes[i].state == READY) {
			return i;
		}
	}

	// Allow to run the same process again, when no other process is ready
	for (i = 0; i <= runningProcess; i++)  {
		if (processes[i].state == READY) {
			return i;
		}
	}

	return INVALID_PROCESS_ID;
}

processId_t getNextProcessIdByState(processState_t state) {
	int i;
	for (i = 0; i < PROCESSES_MAX; i++) {
		if (processes[i].state == state) {
			return i;
		}
	}

	return INVALID_PROCESS_ID;
}

void atomicStart(void) {
	// TODO: stop interrupts
}

void atomicEnd(void) {
	// TODO: activate interrupts
}
