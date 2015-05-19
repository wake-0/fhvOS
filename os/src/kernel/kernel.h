/*
 * kernel.h
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

extern void KernelStart();
extern void KernelVersion(unsigned int* major, unsigned int* minor, unsigned int* patch);
extern long KernelGetUptime();
extern long KernelTick(int ticks);
extern int	KernelInfo(const char *format, ...);
extern int	KernelDebug(const char *format, ...);
extern int	KernelError(const char *format, ...);
extern void KernelExecute(char* command);

#endif /* KERNEL_KERNEL_H_ */
