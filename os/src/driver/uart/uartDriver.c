/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "uartDriver.h"
#include "../../hal/uart/uartHal.h"

void UARTDriverInit(void)
{
	uart_t uart = UART0;
	UARTHALSoftwareReset(uart);
	UARTHALFIFOSettings(uart);

	configuration_t config = { UART_BAUDRATE_9600,
							   UART_PARITY_NONE,
							   UART_CHARLENGTH_8,
							   UART_STOPBIT_ENABLED };
	UARTHALSettings(uart, config);
}

void UARTDriverWrite(char* msg)
{
	uart_t uart = UART0;
	UARTHALFIFOWrite(uart, msg);
}
