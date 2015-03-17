/*
 * driver.h
 *
 *  Created on: 11.03.2015
 *      Author: Nico
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

#define DRIVER_FUNCTION_NOT_SUPPORTED 	-2
#define DRIVER_ERROR					-1
#define DRIVER_OK						0

typedef struct driver_t {
	int (*init)(short);
	int (*open)(short);
	int (*close)(short);
	int (*read)(short, char*, int*);
	int (*write)(short, char*, int);
	int (*ioctl)(short, int, int, char*, int);
} driver_t;

#endif /* DRIVER_DRIVER_H_ */
