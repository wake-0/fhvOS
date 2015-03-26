/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "uartDriver.h"
#include "../beaglebone/hw_uart.h"

void UARTDriverInit(uart_t uart)
{
	UARTHalSoftwareReset(uart);

	configuration_t config;
	config.baudRate = UART_BAUDRATE_9600;
	config.parity =  UART_PARITY_NONE;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_1;

	UARTHalFifoSettings(uart);
	UARTHalSettings(uart, &config);
}

void UARTDriverWrite(uart_t uart, uint8_t* msg)
{
	UARTHalFifoWrite(uart, msg);
}

int UARTDriverInit(uint16_t id) {
	int uartCount = BOARD_UART_COUNT;
	if (id > uartCount - 1) { return DRIVER_ERROR; }

	//Setup the uart
}

int UARTDriverOpen(uint16_t id) {

}

int UARTDriverClose(uint16_t id) {

}

int UARTDriverWrite(uint16_t id, char* buf, uint16_t len) {

}

int UARTDriverRead(uint16_t id, char* buf, uint16_t len) {

}

int UARTDriverIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len) {

}
