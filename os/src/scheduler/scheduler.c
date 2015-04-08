/*
 * scheduler.c
 *
 *  Created on: 01.04.2015
 *      Author: Kevin
 */
#include "scheduler.h"

/*
 * Defines for the process stack start and size
 */
#define STACK_START		(0x40000000)
#define STACK_SIZE		(0x00001000)

/*
 * Register defines
 */
#define R13			(13)

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

	// IMPORTANT: when a task which was interrupted by SWI is scheduled by IRQ the
	// PC must be incremented because PC will be changed by return through SUBS thus prevent to repeat last SWI instruction
	// IMPORTANT: when a task which was interrupted by IRQ is scheduled by SWI the
	// PC must be subtracted because PC was incremented but SWI will not repeat instruction thus decrement PC to repeat
	// CONCLUSION: increment for SWI here and decrement according to systemstate in scheduleNextReady
	processes[freeProcess].context->pc = ((pc_t) func) + 1;
	// CPSR
	// N|Z|C|V|Q|IT|J| DNM| GE | IT   |E|A|I|F|T|  M  |
	// Code | Size | Description
	// N 	| [1]  | Negative/Less than
	// Z 	| [1]  | Zero
	// C 	| [1]  | Carry/Borrow/Extend
	// V 	| [1]  | Overflow
	// Q 	| [1]  | Sticky Overflow
	// IT   | [2]  | IT [1:0]
	// J	| [1]  | Java state bit
	// DNM  | [4]  | Do not modify
	// GE	| [4]  | Greater than or equal to
	// IT	| [6]  | IT [7:2]
	// E	| [1]  | Data endianness bit
	// A	| [1]  | Imprecise abort disable bit
	// I	| [1]  | IRQ disable
	// F	| [1]  | FIQ disable
	// T	| [1]  | Thumb state bit
	// M	| [1]  | M[4:0] Mode bits
	uint32_t userMode = 0b10000;
	processes[freeProcess].context->cpsr = userMode;
	// Let R13 point to the PCB of the running process
	processes[freeProcess].context->registers[R13] = (void*) (STACK_START + STACK_SIZE);

	// TODO: check this atomic end needed
	atomicEnd();
	return SCHEDULER_OK;
}

int SchedulerRunNextProcess(context_t* context) {
	atomicStart();

	processId_t nextProcess = getNextReadyProcessId();
	if (nextProcess == INVALID_PROCESS_ID) {
		atomicEnd();
		return SCHEDULER_ERROR;
	}

	// TODO: Decide what should be done with a running process
	// which is finished or blocked
	if (processes[runningProcess].state == RUNNING) {
		// Set the running process to ready
		processes[runningProcess].state = READY;
		// Save the current context to the running process
		// and then change the running process
		memcpy(processes[runningProcess].context, context, sizeof(context_t));
	}

	// Set the next processes to running
	runningProcess = nextProcess;
	processes[runningProcess].state = RUNNING;
	// Update the context for the next running process
	memcpy(context, processes[runningProcess].context, sizeof(context_t));

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

process_t* SchedulerGetRunningProcess(void) {
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

// Round Robin
processId_t getNextReadyProcessId(void) {
	int i;
	for (i = (runningProcess + 1); i < PROCESSES_MAX; i++) {
		if (processes[i].state == READY) {
			return i;
		}
	}

	// Do not allow to run the same process again, because the running process is not READY
	for (i = 0; i < runningProcess; i++)  {
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
