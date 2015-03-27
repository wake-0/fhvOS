/*
 * uartHal.h
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */

#ifndef UART_HAL_H_
#define UART_HAL_H_

#include "../platform/platform.h"
#include "../board/beaglebone/hw_uart.h"

#define UART_HAL_OK		1
#define UART_HAL_ERROR	0

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
	UART_CHARLENGTH_5 = 0x0,
	UART_CHARLENGTH_6 = 0x1,
	UART_CHARLENGTH_7 = 0x2,
	UART_CHARLENGTH_8 = 0x3
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
extern int UARTHalSoftwareReset(uartPins_t uartPins);
extern int UARTHalFifoSettings(uartPins_t uartPins);
extern int UARTHalSettings(uartPins_t uartPins, configuration_t* config);
extern int UARTHalFifoWrite(uartPins_t uartPins, uint8_t* msg);

#endif /* UART_HAL_H_ */
