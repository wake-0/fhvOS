/*
 * uart.h
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */

#ifndef UART_H_
#define UART_H_

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
	UART_STOPBIT_NONE,
	UART_STOPBIT_ENABLED
} stopbit_t;

/*
 * UART configuration
 */
typedef struct {
	baudrate_t baudeRate;
	parity_t parity;
	charlength_t charLength;
	stopbit_t stopBit;
} configuration_t;

/*
 * functions
 */
void UARTSoftwareReset(uart_t uart);
void UARTFIFOSettings(uart_t uart);
void UARTSettings(uart_t uart, configuration_t config);

#endif /* UART_H_ */
