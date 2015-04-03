/*
 * systemcalls.h
 * File contains the definition of the minix system calls.
 *
 * see http://wiki.minix3.org/en/DevelopersGuide/KernelApi#sys_fork
 *
 *  Created on: 03.04.2015
 *      Author: Marko
*/

#ifndef SYSTEMAPI_SYSTEMCALLS_H_
#define SYSTEMAPI_SYSTEMCALLS_H_

#define EXIT        		1
#define FORK        		2
#define READ        		3
#define WRITE       		4
#define OPEN        		5
#define CLOSE       		6
#define WAIT        		7
#define CREAT       		8
#define LINK        		9
#define UNLINK     	 	  	10
#define WAITPID           	11
#define CHDIR             	12
#define TIME              	13
#define MKNOD             	14
#define CHMOD             	15
#define CHOWN             	16
#define BRK               	17
#define STAT              	18
#define LSEEK             	19
#define GETPID            	20
#define MOUNT             	21
#define UMOUNT            	22
#define SETUID            	23
#define GETUID            	24
#define STIME             	25
#define PTRACE            	26
#define ALARM             	27
#define FSTAT             	28
#define PAUSE             	29
#define UTIME             	30
#define ACCESS            	33
#define SYNC              	36
#define KILL              	37
#define RENAME            	38
#define MKDIR             	39
#define RMDIR             	40
#define DUP               	41
#define PIPE             	42
#define TIMES             	43
#define SETGID            	46
#define GETGID            	47
#define SIGNAL            	48
#define IOCTL             	54
#define FCNTL             	55
#define EXEC              	59
#define UMASK             	60
#define CHROOT            	61
#define SETSID            	62
#define GETPGRP           	63

 // The following are not system calls, but are processed like them.

#define UNPAUSE           	65    // to MM or FS: check for EINTR
#define REVIVE            	67    // to FS: revive a sleeping process
#define TASK_REPLY        	68    // to FS: reply code from tty task

 // Posix signal handling

#define SIGACTION         	71
#define SIGSUSPEND        	72
#define SIGPENDING        	73
#define SIGPROCMASK       	74
#define SIGRETURN         	75
#define REBOOT            	76    // to PM

 // MINIX specific calls, e.g., to support system services.

#define SVRCTL            	77
#define GETSYSINFO        	79    // to PM or FS
#define GETPROCNR         	80    // to PM
#define DEVCTL            	81    // to FS
#define FSTATFS           	82    // to FS
#define FREEMEM           	84    // to PM
#define SELECT            	85    // to FS
#define FCHDIR            	86    // to FS
#define FSYNC             	87    // to FS
#define GETPRIORITY       	88    // to PM
#define SETPRIORITY       	89    // to PM
#define GETTIMEOFDAY      	90    // to PM

typedef struct
{
	int arg1, arg2, arg3;
	char * arg4, * arg5, * arg6;
} messageArgsType1_t;

typedef struct
{
	int arg1, arg2, arg3;
	long arg4, arg5;
	char * arg6;
} messageArgsType2_t;

typedef struct
{
	int arg1, arg2;
	char * arg3, * arg4;
} messageArgsType3_t;

typedef struct
{
	unsigned int arg1, arg2, arg3, arg4;
	char * arg5;
} messageArgsType4_t;

typedef struct
{
	int arg1, arg2;
	short arg3, arg4;
	char * arg5;
} messageArgsType5_t;

typedef struct
{
	int arg1, arg2, arg3, arg4;
	char * arg5, * arg6;
} messageArgsType6_t;

typedef struct
{
	int arg1, arg2;
	char * arg3, * arg4, * arg5, * arg6;
} messageArgsType7_t;

typedef struct
{
	int messageSource;
	int messageType;
	union
	{
		messageArgsType1_t args1;
		messageArgsType2_t args2;
		messageArgsType3_t args3;
		messageArgsType4_t args4;
		messageArgsType5_t args5;
		messageArgsType6_t args6;
		messageArgsType7_t args7;
	}
	messageArgs;
} systemCallMessage_t;

extern void SystemCall(int systemCallNumber, int destination, systemCallMessage_t message);

#endif // SYSTEMAPI_SYSTEMCALLS_H_
