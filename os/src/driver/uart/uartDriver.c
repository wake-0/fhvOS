/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "uartDriver.h"

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
