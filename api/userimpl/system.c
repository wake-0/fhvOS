/*
 * system.c
 *
 *  Created on: 22.05.2015
 *      Author: Nicolaj Hoess
 */

#include "../includes/system.h"
#include "../includes/systemcall.h"
#include <stdio.h>
#include <string.h>

/*
 * \brief This function executes the given command. The command may contain additional arguments.
 * 		  The function returns SYSTEM_FAILURE if the command has not been executed or a positive number
 * 		  which corresponds to the newly created process's pid.
 */
int execute(char* command)
{
	int returnArg = 0;
	// Package the command into a message_t struct
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_EXEC;
	message.messageArgs.callArg = strlen(command);
	message.messageArgs.callBuf = command;

	message.messageArgs.returnArg = &returnArg;

	SystemCall(&message);

	return returnArg;
}

/*
 * \brief Switches to the next process
 */
void yield(void)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_YIELD;

	SystemCall(&message);
}

/*
 * \brief Quits the current process
 */
void overriden__exit(int value)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_EXIT;
	message.messageArgs.callArg = value;

	SystemCall(&message);
}

/*
 * \brief Sets the process to sleeping for the given amount of milliseconds
 */
void sleep(unsigned int milliseconds)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_SLEEP;
	message.messageArgs.callArg = milliseconds;

	SystemCall(&message);
}

void print(const char* msg, int len)
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_PRINT;
	message.messageArgs.callArg = len;
	message.messageArgs.callBuf = (char*) msg;

	SystemCall(&message);
}

long uptime()
{
	systemCallMessage_t message;
	message.systemCallNumber = SYSTEM_CALL_TIME;

	char returnVal[4]; // 4 bytes to store long // TODO I don't know why long is only 32 bit but anyway this constant should be extracted
	message.messageArgs.returnBuf = returnVal;

	SystemCall(&message);

	long result = 0;
	memcpy(&result, message.messageArgs.returnBuf, 4);
	return result;
}
