/*
 * uartDriver.h
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */

#ifndef DRIVER_UART_UARTDRIVER_H_
#define DRIVER_UART_UARTDRIVER_H_

#include "../../hal/platform/platform.h"
#include "../../hal/uart/uartHal.h"
#include "../driver.h"

void UARTDriverInit(uart_t);
void UARTDriverWrite(uart_t, uint8_t* msg);

extern int UARTDriverInit(uint16_t id);
extern int UARTDriverOpen(uint16_t id);
extern int UARTDriverClose(uint16_t id);
extern int UARTDriverWrite(uint16_t id, char* buf, uint16_t len);
extern int UARTDriverRead(uint16_t id, char* buf, uint16_t len);
extern int UARTDriverIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len);

#endif /* DRIVER_UART_UARTDRIVER_H_ */
