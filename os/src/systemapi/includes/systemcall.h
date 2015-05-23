/*
 * systemcalls.h
 * File contains the definition of the minix system calls.
 *
 * see http://wiki.minix3.org/en/DevelopersGuide/KernelApi#SYSTEM_CALL_fork
 *
 *  Created on: 03.04.2015
 *      Author: Marko
*/

#ifndef SYSTEMAPI_SYSTEMCALLS_H_
#define SYSTEMAPI_SYSTEMCALLS_H_

#define SYSTEM_CALL_EXIT        		1
#define SYSTEM_CALL_FORK        		2
#define SYSTEM_CALL_READ        		3
#define SYSTEM_CALL_WRITE       		4
#define SYSTEM_CALL_OPEN        		5
#define SYSTEM_CALL_CLOSE       		6
#define SYSTEM_CALL_WAIT        		7
#define SYSTEM_CALL_CREAT       		8
#define SYSTEM_CALL_LINK        		9
#define SYSTEM_CALL_UNLINK     	 	  	10
#define SYSTEM_CALL_WAITPID           	11
#define SYSTEM_CALL_CHDIR             	12
#define SYSTEM_CALL_TIME              	13
#define SYSTEM_CALL_MKNOD             	14
#define SYSTEM_CALL_CHMOD             	15
#define SYSTEM_CALL_CHOWN             	16
#define SYSTEM_CALL_BRK               	17
#define SYSTEM_CALL_STAT              	18
#define SYSTEM_CALL_LSEEK             	19
#define SYSTEM_CALL_GETPID            	20
#define SYSTEM_CALL_MOUNT             	21
#define SYSTEM_CALL_UMOUNT            	22
#define SYSTEM_CALL_SETUID            	23
#define SYSTEM_CALL_GETUID            	24
#define SYSTEM_CALL_STIME             	25
#define SYSTEM_CALL_PTRACE            	26
#define SYSTEM_CALL_ALARM             	27
#define SYSTEM_CALL_FSTAT             	28
#define SYSTEM_CALL_PAUSE             	29
#define SYSTEM_CALL_UTIME             	30
#define SYSTEM_CALL_ACCESS            	33
#define SYSTEM_CALL_SYNC              	36
#define SYSTEM_CALL_KILL              	37
#define SYSTEM_CALL_RENAME            	38
#define SYSTEM_CALL_MKDIR             	39
#define SYSTEM_CALL_RMDIR             	40
#define SYSTEM_CALL_DUP               	41
#define SYSTEM_CALL_PIPE             	42
#define SYSTEM_CALL_TIMES             	43
#define SYSTEM_CALL_SETGID            	46
#define SYSTEM_CALL_GETGID            	47
#define SYSTEM_CALL_SIGNAL            	48
#define SYSTEM_CALL_IOCTL             	54
#define SYSTEM_CALL_FCNTL             	55
#define SYSTEM_CALL_EXEC              	59
#define SYSTEM_CALL_UMASK             	60
#define SYSTEM_CALL_CHROOT            	61
#define SYSTEM_CALL_SETSID            	62
#define SYSTEM_CALL_GETPGRP           	63

#define __SYSTEM_CALL_STR_HELPER(x) #x
#define SYSTEM_CALL_ASM_PREFIX			"    SWI   #"
#define SYSTEM_CALL_ASM(x)				SYSTEM_CALL_ASM_PREFIX __SYSTEM_CALL_STR_HELPER(x)

typedef struct
{
	int callArg;
	char* callBuf;
	int* returnArg;
	char* returnBuf;
} messageArgs_t;

typedef struct
{
	int systemCallNumber;
	messageArgs_t messageArgs;
} systemCallMessage_t;

extern int SystemCall(systemCallMessage_t * message);

#endif // SYSTEMAPI_SYSTEMCALLS_H_
