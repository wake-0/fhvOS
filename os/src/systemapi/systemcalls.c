/*
 * systemcalls.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */


#include "systemcalls.h"
#include "systemcallFunctions.h"


void SystemCall(systemCallMessage_t * message)
{
	switch(message->systemCallNumber)
	{
		case SYS_EXIT:
			break;
		case SYS_FORK:
			break;
		case SYS_READ:
			break;
		case SYS_WRITE:
			break;
		case SYS_OPEN:
			break;
		case SYS_CLOSE:
			break;
		case SYS_WAIT:
			break;
		case SYS_CREAT:
			break;
		case SYS_LINK:
			break;
		case SYS_UNLINK:
			break;
		case SYS_WAITPID:
			break;
		case SYS_CHDIR:
			break;
		case SYS_TIME:
			break;
		case SYS_MKNOD:
			break;
		case SYS_CHMOD:
			SystemCallChangeMode(message->messageArgs.arg1);
			break;
		case SYS_CHOWN:
			break;
		case SYS_BRK:
			break;
		case SYS_STAT:
			break;
		case SYS_LSEEK:
			break;
		case SYS_GETPID:
			break;
		case SYS_MOUNT:
			break;
		case SYS_UMOUNT:
			break;
		case SYS_SETUID:
			break;
		case SYS_GETUID:
			break;
		case SYS_STIME:
			break;
		case SYS_PTRACE:
			break;
		case SYS_ALARM:
			break;
		case SYS_FSTAT:
			break;
		case SYS_PAUSE:
			break;
		case SYS_UTIME:
			break;
		case SYS_ACCESS:
			break;
		case SYS_SYNC:
			break;
		case SYS_KILL:
			break;
		case SYS_RENAME:
			break;
		case SYS_MKDIR:
			break;
		case SYS_RMDIR:
			break;
		case SYS_DUP:
			break;
		case SYS_PIPE:
			break;
		case SYS_TIMES:
			break;
		case SYS_SETGID:
			break;
		case SYS_GETGID:
			break;
		case SYS_SIGNAL:
			break;
		case SYS_IOCTL:
			break;
		case SYS_FCNTL:
			break;
		case SYS_EXEC:
			break;
		case SYS_UMASK:
			break;
		case SYS_CHROOT:
			break;
		case SYS_SETSID:
			break;
		case SYS_GETPGRP:
			break;
	}
}
