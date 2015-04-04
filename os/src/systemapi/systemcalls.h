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

#define SYS_EXIT        		1
#define SYS_FORK        		2
#define SYS_READ        		3
#define SYS_WRITE       		4
#define SYS_OPEN        		5
#define SYS_CLOSE       		6
#define SYS_WAIT        		7
#define SYS_CREAT       		8
#define SYS_LINK        		9
#define SYS_UNLINK     	 	  	10
#define SYS_WAITPID           	11
#define SYS_CHDIR             	12
#define SYS_TIME              	13
#define SYS_MKNOD             	14
#define SYS_CHMOD             	15
#define SYS_CHOWN             	16
#define SYS_BRK               	17
#define SYS_STAT              	18
#define SYS_LSEEK             	19
#define SYS_GETPID            	20
#define SYS_MOUNT             	21
#define SYS_UMOUNT            	22
#define SYS_SETUID            	23
#define SYS_GETUID            	24
#define SYS_STIME             	25
#define SYS_PTRACE            	26
#define SYS_ALARM             	27
#define SYS_FSTAT             	28
#define SYS_PAUSE             	29
#define SYS_UTIME             	30
#define SYS_ACCESS            	33
#define SYS_SYNC              	36
#define SYS_KILL              	37
#define SYS_RENAME            	38
#define SYS_MKDIR             	39
#define SYS_RMDIR             	40
#define SYS_DUP               	41
#define SYS_PIPE             	42
#define SYS_TIMES             	43
#define SYS_SETGID            	46
#define SYS_GETGID            	47
#define SYS_SIGNAL            	48
#define SYS_IOCTL             	54
#define SYS_FCNTL             	55
#define SYS_EXEC              	59
#define SYS_UMASK             	60
#define SYS_CHROOT            	61
#define SYS_SETSID            	62
#define SYS_GETPGRP           	63

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
	unsigned int arg4, arg5;
	char * arg6, *arg7;
} messageArgs_t;


typedef struct
{
	int systemCallNumber;
	int messageType;
	messageArgs_t messageArgs;
} systemCallMessage_t;

extern int SystemCall(systemCallMessage_t * message);

#endif // SYSTEMAPI_SYSTEMCALLS_H_
