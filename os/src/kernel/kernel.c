/*
 * kernel.c
 *
 *  Created on: 14.05.2015
 *      Author: Marko Petrovic
 *      Contr.: Nicolaj Hoess
 */

#include "kernel.h"

static long uptimeTicks = 0;

long KernelGetUptime()
{
	return uptimeTicks;
}

long KernelTick(int ticks)
{
	return (uptimeTicks += ticks);
}
