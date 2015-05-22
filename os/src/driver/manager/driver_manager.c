/*
 * manager.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "driver_manager.h"

#include <stdlib.h>

#include "../cpu/driver_cpu.h"
#include "../led/driver_led.h"
#include "../uart/driver_uart.h"
#include "../timer/driver_timer.h"
#include "../sdcard/driver_sdcard.h"

#define MAX_DRIVER		255

static driver_t* drivers[MAX_DRIVER];

void DriverManagerInit()
{
	// Create all drivers

	// CPU Driver
	driver_t* cpu = malloc(sizeof(driver_t));
	cpu->init = &CPUInit;
	cpu->open = &CPUOpen;
	cpu->close = &CPUClose;
	cpu->read = &CPURead;
	cpu->write = &CPUWrite;
	cpu->ioctl = &CPUIoctl;
	drivers[DRIVER_ID_CPU] = cpu;

	// LED Driver
	driver_t* led = malloc(sizeof(driver_t));
	led->init = &LEDInit;
	led->open = &LEDOpen;
	led->close = &LEDClose;
	led->read = &LEDRead;
	led->write = &LEDWrite;
	led->ioctl = &LEDIoctl;
	drivers[DRIVER_ID_LED] = led;

	// UART Driver
	driver_t* uart = malloc(sizeof(driver_t));
	uart->init = &UARTDriverInit;
	uart->open = &UARTDriverOpen;
	uart->close = &UARTDriverClose;
	uart->read = &UARTDriverRead;
	uart->write = &UARTDriverWrite;
	uart->ioctl = &UARTDriverIoctl;
	drivers[DRIVER_ID_UART] = uart;

	// TIMER Driver
	driver_t* timer = malloc(sizeof(driver_t));
	timer->init = &TimerDriverInit;
	timer->open = &TimerDriverOpen;
	timer->close = &TimerDriverClose;
	timer->write = &TimerDriverWrite;
	timer->read = &TimerDriverRead;
	timer->ioctl = &TimerDriverIoctl;
	drivers[DRIVER_ID_TIMER] = timer;

	// SDCARD Driver

	driver_t* sdcard = malloc(sizeof(driver_t));
	sdcard->init = &SDCardInit;
	sdcard->open = &SDCardOpen;
	sdcard->close = &SDCardClose;
	sdcard->write = &SDCardWrite;
	sdcard->read = &SDCardRead;
	sdcard->ioctl = &SDCardIoctl;
	drivers[DRIVER_ID_SDCARD] = sdcard;
}

driver_t* DriverManagerGetDriver(driver_id_t driver_id)
{
	return drivers[driver_id];
}

void DriverManagerDestruct()
{
	int i;
	for (i = 0; i < MAX_DRIVER; i++) {
		if (drivers[i] != NULL) {
			free(drivers[i]);
			drivers[i] = NULL;
		}
	}
}
