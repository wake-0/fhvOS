/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "driver_uart.h"

#include "../beaglebone/hw_uart.h"

int UARTDriverInit(uint16_t id) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }

	//Setup the uart
	uartPins_t pins = GetUARTPins(id);
	UARTHalSoftwareReset(pins);

	configuration_t config;
	config.baudRate = UART_BAUDRATE_9600;
	config.parity =  UART_PARITY_0;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_1;

	UARTHalFifoSettings(pins);
	UARTHalSettings(pins, &config);
	return DRIVER_OK;
}

int UARTDriverOpen(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverClose(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverWrite(uint16_t id, char* buf, uint16_t len) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }
	uartPins_t pins = GetUARTPins(id);

	// Write chunks
	int i;
	uint8_t msg;
	for (i = 0; i < len; i++) {
		msg = (uint8_t)*(buf+i);
		UARTHalFifoWrite(pins, &msg);
	}

	return DRIVER_OK;
}

int UARTDriverRead(uint16_t id, char* buf, uint16_t len) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }
	uartPins_t pins = GetUARTPins(id);

	// Read chunks
	int i;
	for (i = 0; i < len; i++) {
		UARTHalFifoRead(pins, (uint8_t*)(buf+i));
	}

	return DRIVER_OK;
}

int UARTDriverIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len) {
	return DRIVER_OK;
}
