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
typedef enum {UART0, UART1, UART2, UART3, UART4, UART5} uart_t;

void UARTSoftwareReset(uart_t uart);
void UARTFIFOSettings(uart_t uart);
void UARTSettings(uart_t uart);

#endif /* UART_H_ */
