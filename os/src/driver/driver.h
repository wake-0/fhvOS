/*
 * driver.h
 *
 *  Created on: 11.03.2015
 *      Author: Nico
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

#include "../hal/platform/platform.h"

#define DRIVER_FUNCTION_NOT_SUPPORTED 	-2
#define DRIVER_ERROR					-1
#define DRIVER_OK						0

typedef struct {
	int (*init)(uint16_t pin);
	int (*open)(uint16_t pin);
	int (*close)(uint16_t pin);
	int (*read)(uint16_t pin, char* buf, uint16_t length);
	int (*write)(uint16_t pin, char* buf, uint16_t length);
	int (*ioctl)(uint16_t pin, uint16_t cmd, uint8_t mode, char* buf, uint16_t length);
} driver_t;

#endif /* DRIVER_DRIVER_H_ */
