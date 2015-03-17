/*
 * led.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "led.h"
#include "../../hal/gpio/gpio.h"

int LEDInit	(short id)
{
	int ledCount = GPIOGetLedCount();
	if (id > ledCount - 1) return DRIVER_ERROR;
	// Set up the GPIO pin
	GPIOEnable(GPIOGetLedPin(id));
	GPIOReset(GPIOGetLedPin(id));
	GPIOSetMux(GPIOGetLedPin(id), MUX_MODE_LED);
	GPIOSetPinDirection(GPIOGetLedPin(id), PIN_DIRECTION_OUT);
	return DRIVER_OK;
}

int LEDOpen	(short id)
{
	int ledCount = GPIOGetLedCount();
	if (id > ledCount - 1) return DRIVER_ERROR;
	return DRIVER_OK;
}

int LEDClose (short id)
{
	// We turn off the led
	char buf[1] = { 0 };
	return LEDWrite(id, &buf[0], 1);
}

int LEDWrite (short id, char* buf, int len)
{
	int ledCount = GPIOGetLedCount();
	if (id > ledCount - 1) return DRIVER_ERROR;

	if (len != 1) return DRIVER_ERROR;
	if (buf[0] == 1) GPIOSetPinValue(GPIOGetLedPin(id), PIN_VALUE_HIGH);
	else if (buf[0] == 0) GPIOSetPinValue(GPIOGetLedPin(id), PIN_VALUE_LOW);
	else return DRIVER_ERROR;
	return DRIVER_OK;
}

int LEDRead	(short id, char* buf, int* len)
{
	// TODO Implement this
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}

int LEDIoctl (short id, int cmd, int mode, char* buf, int len)
{
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}
