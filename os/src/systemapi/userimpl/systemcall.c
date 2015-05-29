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
	case SYSTEM_CALL_YIELD:
	    asm(SYSTEM_CALL_ASM(SYSTEM_CALL_YIELD));
		break;
	case SYSTEM_CALL_EXIT:
	    asm(SYSTEM_CALL_ASM(SYSTEM_CALL_EXIT));
		break;
	case SYSTEM_CALL_SLEEP:
	    asm(SYSTEM_CALL_ASM(SYSTEM_CALL_SLEEP));
		break;
	case SYSTEM_CALL_READ:
	    asm(SYSTEM_CALL_ASM(SYSTEM_CALL_READ));
		break;
	case SYSTEM_CALL_CWD:
		asm(SYSTEM_CALL_ASM(SYSTEM_CALL_CWD));
		break;
	case SYSTEM_CALL_READ_DIR:
		asm(SYSTEM_CALL_ASM(SYSTEM_CALL_READ_DIR));
		break;
	case SYSTEM_CALL_CHDIR:
		asm(SYSTEM_CALL_ASM(SYSTEM_CALL_CHDIR));
		break;
	}

	return SYSTEM_CALL_OK;
}
