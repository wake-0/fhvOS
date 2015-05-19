/*
 * systemcallFunctions.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#include "systemcallFunctions.h"
#include "../kernel/kernel.h"
#include <stdio.h>
/**
 * \brief	Handles system calls. This function is called by the assembler swi_handler function
 * 			in interrupt.asm.
 */
void SystemCallHandler(systemCallMessage_t* message, unsigned int systemCallNumber, context_t* context)
{
	KernelDebug("Systemcall number=%i\n", message->systemCallNumber);
	KernelDebug("message arg=%s\n", message->messageArgs.buf);
	switch (message->systemCallNumber)
	{
		case SYSTEM_CALL_EXEC:
		{
			// Disassemble argument package
			char* command = message->messageArgs.buf;
			KernelExecute(command);
			break;
		}
		default:
			break;
	}
}
