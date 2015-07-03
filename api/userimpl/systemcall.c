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
	if (message->systemCallNumber >= 0)
	{
		asm(SYSTEM_CALL_ASM_PREFIX "1337");
		return SYSTEM_CALL_OK;
	}

	return SYSTEM_CALL_FAILURE;
}
