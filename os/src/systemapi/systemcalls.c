/*
 * systemcalls.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */


#include "systemcalls.h"



void SystemCall(int systemCallNumber, int destination, systemCallMessage_t message)
{
	switch(systemCallNumber)
	{
		case EXIT:
			break;
		case FORK:
			break;
		case READ:
			break;
		case WRITE:
			break;
		case OPEN:
			break;
		case CLOSE:
			break;
		case WAIT:
			break;
		case CREAT:
			break;
		case LINK:
			break;
		case UNLINK:
			break;
		case WAITPID:
			break;
		case CHDIR:
			break;
		case TIME:
			break;
		case MKNOD:
			break;
		case CHMOD:
			break;
		case CHOWN:
			break;
		case BRK:
			break;
		case STAT:
			break;
		case LSEEK:
			break;
		case GETPID:
			break;
		case MOUNT:
			break;
		case UMOUNT:
			break;
		case SETUID:
			break;
		case GETUID:
			break;
		case STIME:
			break;
		case PTRACE:
			break;
		case ALARM:
			break;
		case FSTAT:
			break;
		case PAUSE:
			break;
		case UTIME:
			break;
		case ACCESS:
			break;
		case SYNC:
			break;
		case KILL:
			break;
		case RENAME:
			break;
		case MKDIR:
			break;
		case RMDIR:
			break;
		case DUP:
			break;
		case PIPE:
			break;
		case TIMES:
			break;
		case SETGID:
			break;
		case GETGID:
			break;
		case SIGNAL:
			break;
		case IOCTL:
			break;
		case FCNTL:
			break;
		case EXEC:
			break;
		case UMASK:
			break;
		case CHROOT:
			break;
		case SETSID:
			break;
		case GETPGRP:
			break;
	}
}
