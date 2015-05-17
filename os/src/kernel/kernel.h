/*
 * kernel.h
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

extern long KernelGetUptime();
extern long KernelTick(int ticks);
extern int	KernelInfo(const char *format, ...);
extern int	KernelDebug(const char *format, ...);
extern int	KernelError(const char *format, ...);

#endif /* KERNEL_KERNEL_H_ */
