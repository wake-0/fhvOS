/*
 * led.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include <hw_led.h>
#include "led.h"
#include "../../hal/gpio/gpio.h"

int LEDInit	(short id)
{
	int ledCount = BOARD_LED_COUNT;
	if (id > ledCount - 1) return DRIVER_ERROR;
	// Set up the GPIO pin
	GPIOEnable(BOARD_LED(id));
	GPIOReset(BOARD_LED(id));
	GPIOSetMux(BOARD_LED(id), MUX_MODE_LED);
	GPIOSetPinDirection(BOARD_LED(id), PIN_DIRECTION_OUT);
	return DRIVER_OK;
}

int LEDOpen	(short id)
{
	int ledCount = BOARD_LED_COUNT;
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
	int ledCount = BOARD_LED_COUNT;
	if (id > ledCount - 1) return DRIVER_ERROR;

	if (len != 1) return DRIVER_ERROR;

	switch(buf[0])
	{
		case 1:
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_HIGH);
			break;
		case 0:
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_LOW);
			break;
		default:
			return DRIVER_ERROR;
	}
	return DRIVER_OK;
}

int LEDRead	(short id, char* buf, int len)
{
	// TODO Implement this
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}

int LEDIoctl (short id, int cmd, int mode, char* buf, int len)
{
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}
