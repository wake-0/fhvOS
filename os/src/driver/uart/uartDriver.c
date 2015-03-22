/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "uartDriver.h"
#include "../../hal/uart/uartHal.h"

void UARTDriverInit(uart_t uart)
{
	UARTHALSoftwareReset(uart);
	UARTHALFIFOSettings(uart);

	configuration_t config = { UART_BAUDRATE_9600,
							   UART_PARITY_NONE,
							   UART_CHARLENGTH_8,
							   UART_STOPBIT_1 };
	UARTHALSettings(uart, config);
}

void UARTDriverWrite(uart_t uart, char* msg)
{
	UARTHALFIFOWrite(uart, msg);
}
