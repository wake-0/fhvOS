/*
 * hw_uart.c
 *
 *  Created on: 27.03.2015
 *      Author: Kevin
 */
#include "hw_uart.h"

// TODO: hasmap with all uart settings
// return only the setting which is defined in the uart hasmap
uartPins_t GetUARTPins(uint16_t uartNr) {
	uartPins_t pins;
	switch (uartNr) {
	case 0:
		pins.txd = BOARD_UART0_TXD;
		pins.rxd = BOARD_UART0_RXD;
		break;
	case 1:
		pins.txd = BOARD_UART1_TXD;
		pins.rxd = BOARD_UART1_RXD;
		break;
	case 2:
		pins.txd = BOARD_UART2_TXD;
		pins.rxd = BOARD_UART2_RXD;
		break;
	case 4:
		pins.txd = BOARD_UART4_TXD;
		pins.rxd = BOARD_UART4_RXD;
		break;
	case 5:
		pins.txd = BOARD_UART5_TXD;
		pins.rxd = BOARD_UART5_RXD;
		break;
	default:
		pins.txd = BOARD_UART_UNDEF;
		pins.rxd = BOARD_UART_UNDEF;
	}
	return pins;
}

