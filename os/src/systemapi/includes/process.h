/*
 * process.h
 *
 *  Created on: 30.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef SYSTEMAPI_INCLUDES_PROCESS_H_
#define SYSTEMAPI_INCLUDES_PROCESS_H_

#define PROCESS_MANAGER_MAX_PROCESS_LENGTH		(15)
#define PROCESS_READY							(0)
#define PROCESS_RUNNING							(1)
#define PROCESS_BLOCKED							(2)
#define PROCESS_SLEEPING						(4)

typedef struct
{
	char processName[PROCESS_MANAGER_MAX_PROCESS_LENGTH];
	long startTime;
	int processID;
	int state;
} processInfoAPI_t;

extern int get_process_count(void);
extern int read_processes(processInfoAPI_t* buf, int len);

#endif /* SYSTEMAPI_INCLUDES_PROCESS_H_ */
