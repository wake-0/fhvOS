/*
 * hw_uart.h
 *
 *  Created on: 26.03.2015
 *      Author: Kevin
 */

#ifndef HAL_BOARD_BEAGLEBONE_HW_UART_H_
#define HAL_BOARD_BEAGLEBONE_HW_UART_H_

#include "../../../platform/platform.h"

typedef struct {
	int16_t txd;
	int16_t rxd;
} uartPins_t;

#define	BOARD_UART_COUNT		(5)

// UART undefined pin
#define BOARD_UART_UNDEF		(-1)

// UART TXD registers
#define BOARD_UART0_TXD			(0)
#define BOARD_UART1_TXD			(24)
#define BOARD_UART2_TXD			(21)
#define BOARD_UART4_TXD			(13)
#define BOARD_UART5_TXD			(37)

// UART RXD registers
#define BOARD_UART0_RXD			(0)
#define BOARD_UART1_RXD			(26)
#define BOARD_UART2_RXD			(22)
#define BOARD_UART4_RXD			(11)
#define BOARD_UART5_RXD			(38)

// Get UART pins
extern uartPins_t GetUARTPins(uint16_t uartNr);

#endif /* HAL_BOARD_BEAGLEBONE_HW_UART_H_ */
