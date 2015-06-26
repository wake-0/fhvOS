/*
 * driver_dmx.c
 *
 *  Created on: 05.06.2015
 *      Author: Kevin
 */

#include "driver_dmx.h"
#include "../../devicemanager/devicemanager.h"
#include "../../hal/gpio/hal_gpio.h"

#define DMX_MAX 128 // max. number of DMX data packages.

static device_t uart;
char DMXBuffer[DMX_MAX];


int DMXInit(uint16_t id) {

	// Enable gpio for the dmx module
	// TODO: find the right pin
	/*
	uint16_t pin = 0;
	GPIOEnable(pin);
	GPIOSetPinDirection(pin, PIN_DIRECTION_OUT);
	GPIOSetPinValue(pin, PIN_VALUE_HIGH);
	*/
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
	uart = *((device_t*)buf);

	// TODO: check uart == nullpointer

	int i;

	// Fill buffer with test data
	DMXBuffer[0] = 0;
	for (i = 1; i < DMX_MAX; i++) {
		DMXBuffer[i] = 170;
	}

	DeviceManagerInit(uart);
	DeviceManagerOpen(uart);
	// Write start byte
	DeviceManagerWrite(uart, &DMXBuffer[0], 1);

	// Write test data
	for(i = 1; i < DMX_MAX; i = (i + 1) % DMX_MAX) {
		DeviceManagerWrite(uart, &DMXBuffer[i], sizeof(char));

	}

	// Disable uart
	DeviceManagerIoctl(uart, 2, 0, NULL, 0);

	return DRIVER_OK;
}

