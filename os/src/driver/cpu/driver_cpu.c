/*
 * driver_cpu.c
 *
 *  Created on: 08.04.2015
 *      Author: Nicolaj Hoess
 */

#include "driver_cpu.h"

#include "../../hal/cpu/hal_cpu.h"
#include "../../hal/interrupt/hal_interrupt.h"

int CPUInit(uint16_t id)
{
	if (id != 0) { return DRIVER_ERROR; }

	return DRIVER_OK;
}

int CPUOpen(uint16_t id)
{
	if (id != 0) { return DRIVER_ERROR; }

	return DRIVER_OK;
}

int CPUClose(uint16_t id)
{
	if (id != 0) { return DRIVER_ERROR; }

	return DRIVER_OK;
}

int CPUWrite(uint16_t id, char* buf, uint16_t len)
{
	if (id != 0) { return DRIVER_ERROR; }

	return DRIVER_OK;
}

int CPURead(uint16_t id, char* buf, uint16_t len)
{
	if (id != 0) { return DRIVER_ERROR; }

	return DRIVER_OK;
}

int CPUIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len){
	if (id != 0) { return DRIVER_ERROR; }

	switch (cmd) {
		case DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE:
			InterruptMasterIRQEnable();
			return DRIVER_OK;
		case DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC:
			InterruptResetAINTC();
			return DRIVER_OK;
		default:
			break;
	}
	return DRIVER_ERROR;
}


