/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "uartDriver.h"
#include "../beaglebone/hw_uart.h"

int UARTDriverInit(uint16_t id) {
	int uartCount = BOARD_UART_COUNT;
	if (id > uartCount - 1) { return DRIVER_ERROR; }

	//Setup the uart
	UARTHalSoftwareReset(UART0);

	configuration_t config;
	config.baudRate = UART_BAUDRATE_9600;
	config.parity =  UART_PARITY_NONE;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_1;

	UARTHalFifoSettings(UART0);
	UARTHalSettings(UART0, &config);
	return DRIVER_OK;
}

int UARTDriverOpen(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverClose(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverWrite(uint16_t id, char* buf, uint16_t len) {
	//TODO: use buf
	uint8_t msg = 'a';
	UARTHalFifoWrite(UART0, &msg);
	return DRIVER_OK;
}

int UARTDriverRead(uint16_t id, char* buf, uint16_t len) {
	return DRIVER_OK;
}

int UARTDriverIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len) {
	return DRIVER_OK;
}
