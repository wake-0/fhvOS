/*
 * gpio.c
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#include "gpio.h"
#include "../platform/platform.h"

// Platform dependant includes
#include "../am335x/soc_AM335x.h"
#include "../am335x/hw_gpio_v2.h"

#define LED_COUNT		4

extern void GPIOEnable(bus_number_t busNo)
{

}

extern void GPIODisable(bus_number_t busNo)
{

}

extern void GPIOReset(bus_number_t busNo)
{
	memory_addr_t baseAddr = 0;
	if 		(busNo == 0) baseAddr = SOC_GPIO_0_REGS;
	else if (busNo == 1) baseAddr = SOC_GPIO_1_REGS;
	else if (busNo == 2) baseAddr = SOC_GPIO_2_REGS;
	else if (busNo == 3) baseAddr = SOC_GPIO_3_REGS;
	else return;

	// XXX Prototype how platform/memory is accessed
	// PlatformWriteBitMask(baseAddr + GPIO_SYSCONFIG, GPIO_SYSCONFIG_SOFTRESET, BIT_MASK_ACTION_SET_ON);

	/* Waiting until the GPIO Module is reset.*/
	// while (!PlatformReadBitMask(baseAddr + GPIO_SYSSTATUS, GPIO_SYSSTATUS_RESETDONE, BIT_MASK_ACTION_AND));
}

extern bus_number_t GPIOGetPinBus(int pin)
{

}

extern void GPIOSetMux(int pin, int mux)
{

}

extern void GPIOSetMode(int pin, pin_mode_t mode)
{

}

extern void GPIOSetPinValue(int pin, pin_direction_t dir)
{

}

extern pin_value_t GPIOGetPinValue(int pin, pin_value_t value)
{
	// TODO Implement this
	return PIN_VALUE_LOW;
}

extern int GPIOGetLedPin(unsigned int ledNo)
{
	if (ledNo > LED_COUNT - 1) return UNDEFINED_PIN;
	if (ledNo == 2) return 23;

	// TODO Implement the other led pins
	return UNDEFINED_PIN;
}
extern int GPIOGetLedCount()
{
	return LED_COUNT;
}

