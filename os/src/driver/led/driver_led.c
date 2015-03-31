/*
 * led.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "driver_led.h"

#include <hw_led.h>

#include "../../hal/gpio/hal_gpio.h"

int LEDInit	(uint16_t id)
{
	uint8_t ledCount = BOARD_LED_COUNT;
	if (id > ledCount - 1) return DRIVER_ERROR;
	// Set up the GPIO pin
	GPIOEnable(BOARD_LED(id));
	//GPIOReset(BOARD_LED(id)); // XXX TODO Resetting here causes all prev turned on leds to turn off again. It works without this but maybe its not clean
	GPIOSetMux(BOARD_LED(id), MUX_MODE_LED);
	GPIOSetPinDirection(BOARD_LED(id), PIN_DIRECTION_OUT);
	return DRIVER_OK;
}

int LEDOpen	(uint16_t id)
{
	uint8_t ledCount = BOARD_LED_COUNT;
	if (id > ledCount - 1) return DRIVER_ERROR;
	return DRIVER_OK;
}

int LEDClose (uint16_t id)
{
	// We turn off the led
	char buf[1] = { 0 };
	return LEDWrite(id, &buf[0], 1);
}

int LEDWrite (uint16_t id, char* buf, uint16_t len)
{
	uint8_t ledCount = BOARD_LED_COUNT;
	if (id > ledCount - 1) return DRIVER_ERROR;

	if (len != 1) return DRIVER_ERROR;

	switch(buf[0])
	{
		case '1':
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_HIGH);
			break;
		case '0':
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_LOW);
			break;
		default:
			return DRIVER_ERROR;
	}
	return DRIVER_OK;
}

int LEDRead	(uint16_t id, char* buf, uint16_t len)
{
	// TODO Implement this
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}

int LEDIoctl (uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len)
{
	return DRIVER_FUNCTION_NOT_SUPPORTED;
}
