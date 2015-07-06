/*
 * kernel.h
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include "../scheduler/scheduler.h"

#define KERNEL_VERBOSE_OUTPUT		0
#define	KERNEL_DEBUG_OUTPUT			1
#define KERNEL_ERROR_OUTPUT			1
#define KERNEL_INFO_OUTPUT			1
#define KERNEL_DEFAULT_OUTPUT		1

extern void KernelStart();
extern void KernelVersion(unsigned int* major, unsigned int* minor, unsigned int* patch);
extern long KernelGetUptime();
extern long KernelTick(int ticks);
extern int KernelPrint(const char *format, ...);
extern int KernelVerbose(const char *format, ...);
extern int KernelInfo(const char *format, ...);
extern int KernelDebug(const char *format, ...);
extern int KernelError(const char *format, ...);
extern void KernelExecute(char* command, context_t* context);
extern void KernelAtomicStart(void);
extern void KernelAtomicEnd(void);

#endif /* KERNEL_KERNEL_H_ */
