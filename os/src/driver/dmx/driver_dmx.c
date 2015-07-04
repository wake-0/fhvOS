/*
 * driver_dmx.c
 *
 *  Created on: 05.06.2015
 *      Author: Kevin
 */

#include "driver_dmx.h"
#include "../../devicemanager/devicemanager.h"
#include "../../hal/gpio/hal_gpio.h"
#include "../../kernel/kernel.h"

// Defines
#define DMX_MAX 6 // Max. number of DMX data packages.
#define channel 0

// Static variables
static device_t uart;
static char DMXBuffer[DMX_MAX];
static boolean_t isInitialized;
static uint8_t pin7 = 7;
static uint8_t pin17 = 17;

// Static functions
static void longBreak(device_t* uart);
static void delay(int count);

int DMXInit(uint16_t id) {
	// Enable pin for the dmx module
	GPIOEnable(pin7);
	GPIOSetMux(pin7, MUX_MODE_LED);
	GPIOSetPinDirection(pin7, PIN_DIRECTION_OUT);
	GPIOSetPinValue(pin7, PIN_VALUE_HIGH);

	// Enable pin for the dmx module
	GPIOEnable(pin17);
	GPIOSetMux(pin17, MUX_MODE_LED);
	GPIOSetPinDirection(pin17, PIN_DIRECTION_OUT);
	GPIOSetPinValue(pin17, PIN_VALUE_HIGH);

	// Set the default dmx buffer
	int i = 0;
	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	return DRIVER_OK;
}

int DMXOpen(uint16_t id) {
	return DRIVER_OK;
}

int DMXClose(uint16_t id) {
	return DRIVER_OK;
}

int DMXWrite(uint16_t id, char* buf, uint16_t len) {
	return DRIVER_OK;
}

int DMXRead(uint16_t id, char* buf, uint16_t len) {
	return DRIVER_OK;
}

int DMXIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len) {
	// Get UART from buffer
	uart = *((device_t*) buf);

	// Init uart when necessary
	if (!isInitialized) {
		DeviceManagerInit(uart);
		DeviceManagerOpen(uart);
		isInitialized = true;
	}

	volatile int i;
	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	volatile int direction = 0;
	while (true) {
		longBreak(&uart);

		// TODO: move this stuff
		// sample stuff
		if (direction == 0) {
			DMXBuffer[channel] = DMXBuffer[channel] += 5;
			DMXBuffer[channel+1] = DMXBuffer[channel+1] += 5;
			if (DMXBuffer[channel] == 200) {
				direction = 1;
			}
		} else if (direction == 1) {
			DMXBuffer[channel] = DMXBuffer[channel] -= 5;
			DMXBuffer[channel+1] = DMXBuffer[channel+1] -= 5;
			if (DMXBuffer[channel] == 0) {
				direction = 0;
			}
		}

		DMXBuffer[channel+3] = 5;
		DMXBuffer[channel+2] = 5;


		for (i = 0; i < DMX_MAX; i++) {
			DeviceManagerWrite(uart, &DMXBuffer[i], 1);

			// very important otherwise not everything is written by the uart
			delay(20);
		}

		// break at the end of a communication
		longBreak(&uart);

		// delay between sent commands
		delay(2000);
	}

	return DRIVER_OK;
}

static void longBreak(device_t* uart) {
	// switch to slow baudrate to make a break
	DeviceManagerIoctl(*uart, 2, 2, NULL, 0);

	// send break
	char longbreak = 0x00;
	DeviceManagerWrite(*uart, &longbreak, sizeof(char));

	// create mark
	delay(5000);

	// switch to dmx baudrate
	DeviceManagerIoctl(*uart, 1, 1, NULL, 0);
}

static void delay(int count) {
	volatile int j = 0;
	for (j = 0; j < count; j++);
}
