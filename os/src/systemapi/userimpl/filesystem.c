/*
 * filesystem.c
 *
 *  Created on: 29.05.2015
 *      Author: Nicolaj Hoess
 */


#include "../includes/filesystem.h"
#include "../includes/systemcall.h"

int read_file(char* name, int start, char* buf, int len)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_READ;
	message.messageArgs.callArg = start;
	message.messageArgs.callBuf = name;
	message.messageArgs.returnBuf = buf;
	message.messageArgs.returnArg = &len;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

