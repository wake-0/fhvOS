/*
 * driver_cpu.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DRIVER_CPU_DRIVER_CPU_H_
#define DRIVER_CPU_DRIVER_CPU_H_


#include "../driver.h"

#define DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE		0
#define DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC			1

extern int CPUInit	(uint16_t id);
extern int CPUOpen	(uint16_t id);
extern int CPUClose	(uint16_t id);
extern int CPUWrite	(uint16_t id, char* buf, uint16_t len);
extern int CPURead	(uint16_t id, char* buf, uint16_t len);
extern int CPUIoctl	(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len);

#endif /* DRIVER_CPU_DRIVER_CPU_H_ */
