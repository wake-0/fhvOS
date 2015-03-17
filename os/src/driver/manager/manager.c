/*
 * manager.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "manager.h"
#include <stdlib.h>

#include "../led/led.h"

#define MAX_DRIVER		255

static driver_t* drivers[MAX_DRIVER];

extern void DriverManagerInit()
{
	// Create all drivers

	// LED Driver
	driver_t* led = malloc(sizeof(driver_t));
	led->init = &LEDInit;
	led->open = &LEDOpen;
	led->close = &LEDClose;
	led->read = &LEDRead;
	led->write = &LEDWrite;
	led->ioctl = &LEDIoctl;
	drivers[DRIVER_ID_LED] = led;

}

extern driver_t* DriverManagerGetDriver(driver_id_t driver_id)
{
	return drivers[driver_id];
}
