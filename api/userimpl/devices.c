/*
 * devices.c
 *
 *  Created on: 04.07.2015
 *      Author: Kevin
 */

#include "../includes/devices.h"
#include "../includes/systemcall.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int open_device(char* device) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_OPEN_DEVICE;
	message.messageArgs.callBuf = device;
	int handle = 0;
	message.messageArgs.returnArg = &handle;
	SystemCall(&message);
	return handle;
}

void close_device(int handle) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_CLOSE_DEVICE;
	message.messageArgs.callArg = handle;
	SystemCall(&message);
}

int ioctl_device(int handle, char mode, char cmd, char* buf, int len) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IOCTL_DEVICE;
	message.messageArgs.callArg = len;
	message.messageArgs.callArg2 = handle;
	message.messageArgs.callArg3 = mode;
	message.messageArgs.callArg4 = cmd;
	message.messageArgs.callBuf = buf;

	int res = handle;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);

	return res;
}

int write_device(int handle, int len, char* buf) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_WRITE_DEVICE;

	message.messageArgs.callBuf = buf;
	message.messageArgs.callArg = len;
	int res = handle;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);

	return res;
}
