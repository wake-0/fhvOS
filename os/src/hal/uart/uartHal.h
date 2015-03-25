/*
 * uartHal.h
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */

#ifndef UART_HAL_H_
#define UART_HAL_H_

#include "../platform/platform.h"

/*
 * UART abstraction
 */
typedef enum {
	UART0,
	UART1,
	UART2,
	UART3,
	UART4,
	UART5
} uart_t;

/*
 * UART baudrates
 */
typedef enum {
	UART_BAUDRATE_115200,
	UART_BAUDRATE_57600,
	UART_BAUDRATE_38400,
	UART_BAUDRATE_28800,
	UART_BAUDRATE_19200,
	UART_BAUDRATE_14400,
	UART_BAUDRATE_9600,
	UART_BAUDRATE_4800,
	UART_BAUDRATE_2400,
	UART_BAUDRATE_1200,
	UART_BAUDRATE_1
} baudrate_t;

/**
 * UART_PARITY_NONE: Parity disabled
 * UART_PARITY_0: Paritybit 0 forced
 * UART_PARITY_1: Paritybit 1 forced
 */
typedef enum {
	UART_PARITY_NONE,
	UART_PARITY_0,
	UART_PARITY_1
} parity_t;

/*
 * UART character length
 */
typedef enum {
	UART_CHARLENGTH_5,
	UART_CHARLENGTH_6,
	UART_CHARLENGTH_7,
	UART_CHARLENGTH_8
} charlength_t;

/*
 * UART stopbit
 */
typedef enum {
	UART_STOPBIT_1,
	UART_STOPBIT_2
} stopbit_t;

/*
 * UART configuration
 */
typedef struct {
	baudrate_t baudRate;
	parity_t parity;
	charlength_t charLength;
	stopbit_t stopBit;
} configuration_t;

/*
 * functions
 */
void UARTHALSoftwareReset(uart_t uart);
void UARTHALFIFOSettings(uart_t uart);
void UARTHALSettings(uart_t uart, configuration_t config);
void UARTHALFIFOWrite(uart_t uart, uint8_t* msg);

#endif /* UART_HAL_H_ */
