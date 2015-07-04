/*
 * process.c
 *
 *  Created on: 30.05.2015
 *      Author: Nicolaj Hoess
 */


#include "../includes/process.h"
#include "../includes/systemcall.h"

int get_process_count()
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_GET_PROC_COUNT;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int read_processes(processInfoAPI_t* buf, int len)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_GET_PROC_LIST;
	message.messageArgs.returnBuf = (char*) buf;
	message.messageArgs.callArg = len;
	int res = 0;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int kill_process(int id)
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_KILL;
	message.messageArgs.callArg = id;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

