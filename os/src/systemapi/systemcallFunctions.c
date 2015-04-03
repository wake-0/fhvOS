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
