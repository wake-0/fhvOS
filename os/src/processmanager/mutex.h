/*
 * mutex.h
 *
 *  Created on: 17.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef PROCESSMANAGER_MUTEX_H_
#define PROCESSMANAGER_MUTEX_H_

#include "../scheduler/scheduler.h"

typedef struct struct_pid_queue_t pid_queue_t;

struct struct_pid_queue_t {
	process_t* proc;
	pid_queue_t* next;
};

typedef struct {
	boolean_t locked;
	process_t* owner;
	pid_queue_t* queueHead;
} mutex_t;

extern void MutexLock(mutex_t* mutex);
extern void MutexUnlock(mutex_t* mutex);

#endif /* PROCESSMANAGER_MUTEX_H_ */
