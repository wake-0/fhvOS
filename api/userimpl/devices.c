/*
 * devices.c
 *
 *  Created on: 04.07.2015
 *      Author: Kevin
 */

#include "../includes/devices.h"
#include "../includes/systemcall.h"

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
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int write_device(int handle, int len, char* buf) {
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_WRITE_DEVICE;
	SystemCall(&message);
	return *message.messageArgs.returnArg
}
