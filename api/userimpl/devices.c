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
	message.messageArgs.callArg = len + sizeof(int) + sizeof(char) * 2; // Len of buf + mode + command + handle
	char* callBuf = (char*) malloc(message.messageArgs.callArg);
	memcpy(&callBuf[0], &handle, sizeof(int));
	memcpy(&callBuf[4], &mode, sizeof(char));
	memcpy(&callBuf[5], &cmd, sizeof(char));
	memcpy(&callBuf[6], &buf[0], len);
	message.messageArgs.callBuf = callBuf;

	int res = 0;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	free(callBuf);

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
