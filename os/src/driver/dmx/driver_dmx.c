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

// Static variables
static device_t uart;
static boolean_t isInitialized;
static uint8_t pin7 = 7;
static uint8_t pin17 = 17;
static device_t uart;

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

	// TODO: move this to the high level driver
	uart = DeviceManagerGetDevice("UART1", 5);
	// Init uart when necessary
	if (!isInitialized) {
		DeviceManagerInit(uart);
		DeviceManagerOpen(uart);
		isInitialized = true;
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
	if (len > 12) { return DRIVER_ERROR; }

	volatile int i = 0;
	for (i = 0; i < 2; i++) {
		// start break
		longBreak(&uart);

		for (i = 0; i < len; i++) {
			DeviceManagerWrite(uart, &buf[i], sizeof(char));

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
