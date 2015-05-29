/*
 * systemcallFunctions.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#include "systemapi.h"
#include "../../hal/cpu/hal_cpu.h"
#include "../../kernel/kernel.h"
#include "../../scheduler/scheduler.h"
#include "../../filemanager/filemanager.h"
#include <stdio.h>

/**
 * \brief	Handles system calls. This function is called by the assembler swi_handler function
 * 			in interrupt.asm.
 */
void SystemCallHandler(systemCallMessage_t* message, unsigned int systemCallNumber, context_t* context)
{
	// We allow interrupts but disallow scheduling
	SchedulerDisableScheduling();
	CPUirqe();

	//KernelDebug("Systemcall number=%i\n", message->systemCallNumber);
	switch (message->systemCallNumber)
	{
		case SYSTEM_CALL_EXEC:
		{
			// Disassemble argument package
			char* command = message->messageArgs.callBuf;
			KernelExecute(command, context);
			break;
		}
		case SYSTEM_CALL_YIELD:
		{
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_EXIT:
		{
			int value = message->messageArgs.callArg;
			(void)(value); // Get rid of unused warning TODO Use the return value of the proc
			process_t* curr = SchedulerGetRunningProcess();
			SchedulerKillProcess(curr->id);
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_SLEEP:
		{
			int millis = (unsigned int) message->messageArgs.callArg;
			process_t* curr = SchedulerGetRunningProcess();
			SchedulerSleepProcess(curr->id, millis);
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_READ:
		{
			int res = FileManagerOpenFile(message->messageArgs.callBuf, message->messageArgs.callArg, message->messageArgs.returnBuf, *message->messageArgs.returnArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		default:
			break;
	}
}
