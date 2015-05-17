/*
 * mutex.c
 *
 *  Created on: 17.05.2015
 *      Author: Nicolaj Hoess
 */

#include "mutex.h"
#include "../platform/platform.h"

void MutexLock(mutex_t* mutex)
{
	if (mutex->locked == TRUE)
	{
		// Mutex is locked. Enter the queue and block the process
		process_t* currentProc = SchedulerGetRunningProcess();

		pid_queue_t* newEntry = malloc(sizeof(pid_queue_t));
		newEntry->next = NULL;
		newEntry->proc = currentProc;

		if (mutex->queueHead == NULL)
		{
			mutex->queueHead = newEntry;
		}
		else
		{
			pid_queue_t* curr = mutex->queueHead;
			while (curr->next != NULL)
			{
				curr = curr->next;
			}
			curr->next = newEntry;
		}

		// Block the process
		SchedulerBlockProcess(currentProc->id); // We do not return from this function until the mutex is unlocked

		mutex->queueHead = newEntry->next;

		free(newEntry);
	}

	if (mutex->locked == FALSE)
	{
		// Mutex is unlocked. Lock the mutex and pass
		mutex->locked = TRUE;
		mutex->owner = SchedulerGetRunningProcess();
	}
}

void MutexUnlock(mutex_t* mutex)
{
	if (mutex->locked == FALSE)
	{
		return;
	}

	mutex->locked = FALSE;
	mutex->owner = NULL;
	if (mutex->queueHead != NULL)
	{
		SchedulerUnblockProcess(mutex->queueHead->proc->id);
	}
}
