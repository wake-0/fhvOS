/*
 * ipc.c
 *
 *  Created on: 05.07.2015
 *      Author: Nicolaj Hoess
 */

#include <stdlib.h>
#include <string.h>
#include "../includes/systemcall.h"

int open_ipc_channel(char* namespace_name)
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IPC_OPEN;
	message.messageArgs.callBuf = namespace_name;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int close_ipc_channel(char* namespace_name)
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IPC_CLOSE;
	message.messageArgs.callBuf = namespace_name;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int send_ipc_message(char* namespace_name, char* receiver, char* messageBuf, int len)
{
	int res = len;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IPC_SEND;
	int senderLen = strlen(namespace_name);
	int receiverLen = strlen(receiver);
	char* callBuf = malloc(sizeof(char) * (senderLen + receiverLen) + 4 * sizeof(char));
	if (callBuf == NULL)
	{
		return -1;
	}
	callBuf[0] = (char)senderLen;
	callBuf[1] = (char)receiverLen;
	memcpy(&callBuf[2], namespace_name, senderLen);
	callBuf[2 + senderLen] = '\0';
	memcpy(&callBuf[senderLen+2+1], receiver, receiverLen);
	callBuf[senderLen+2+1+receiverLen] = '\0';
	message.messageArgs.callBuf = callBuf;
	message.messageArgs.returnBuf = messageBuf;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int has_ipc_message(char* namespace_name)
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IPC_CHECK;
	message.messageArgs.callBuf = namespace_name;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	return *message.messageArgs.returnArg;
}

int get_next_ipc_message(char* namespace_name, char* message_buf, int len, char* sender_buffer, int sender_buffer_len)
{
	int res = 0;
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_IPC_GET;
	message.messageArgs.callBuf = namespace_name;
	char* returnBuf = malloc(sizeof(char) * (len + sender_buffer_len) + 4 * sizeof(char));
	if (returnBuf == NULL)
	{
		return -1;
	}
	returnBuf[0] = (char)len;
	returnBuf[1] = (char)sender_buffer_len;
	message.messageArgs.returnBuf = returnBuf;
	message.messageArgs.returnArg = &res;
	SystemCall(&message);
	memcpy(message_buf, &returnBuf[2], len);
	memcpy(sender_buffer, &returnBuf[len+2], sender_buffer_len);
	free(returnBuf);
	return *message.messageArgs.returnArg;
}
