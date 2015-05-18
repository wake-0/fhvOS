/*
 * systemcallFunctions.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#include "systemcallFunctions.h"

void SystemCallChangeMode(int mode)
{
	switch(mode)
	{
		case SYSTEM_MODE:
			CPUSwitchToPrivilegedMode();
			break;
		case USER_MODE:
			CPUSwitchToUserMode();
			break;
	}
}


/**
 * \brief	Handles system calls. This function is called by the assembler swi_handler function
 * 			in interrupt.asm.
 */
void SystemCallHandler(unsigned int systemCallNumber)
{
	// TODO: place system call handling HERE
	// system calls are performed in dependence on the system call number
	// when this function is entered, the operational mode is system mode
	;
}
