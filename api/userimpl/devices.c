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

int open_device(char* device) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_OPEN_DEVICE;
	message.messageArgs.callBuf = device;
	int handle = 0;
	message.messageArgs.returnArg = &handle;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int close_device(int handle) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_CLOSE_DEVICE;
	message.messageArgs.callArg = handle;
	int res = 0;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int ioctl_device(int handle, int mode, int cmd, int len, char* buf) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IOCTL_DEVICE;

	int* callBuf = (int*) malloc(sizeof(int)*4 + len);
	memcmp(&callBuf[0], &handle, sizeof(int));
	memcmp(&callBuf[1], &mode, sizeof(int));
	memcmp(&callBuf[2], &cmd, sizeof(int));
	memcmp(&callBuf[3], &len, sizeof(int));
	memcmp(&callBuf[4], buf, len);

	message.messageArgs.callBuf = (char*)callBuf;
	SystemCall(&message);
	free(callBuf);
	return *message.messageArgs.returnArg;
}

int write_device(int handle, int len, char* buf) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_WRITE_DEVICE;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}
