/*
 * systemcalls.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#include "../includes/systemcall.h"

#define SYSTEM_CALL_OK 1
#define SYSTEM_CALL_FAILURE -1

int SystemCall(systemCallMessage_t* message)
{
	switch(message->systemCallNumber)
	{
	case SYSTEM_CALL_EXEC:
	    asm(SYSTEM_CALL_ASM(SYSTEM_CALL_EXEC));
		break;
	}

	return SYSTEM_CALL_OK;
}
