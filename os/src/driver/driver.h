/*
 * driver.h
 *
 *  Created on: 11.03.2015
 *      Author: Nico
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

typedef struct driver_t {
	// TODO Add return values and parameters
	void (*open)(void);
	void (*close)(void);
	void (*read)(void);
	void (*write)(void);

	// TODO Add ioctl
} Driver;

#endif /* DRIVER_DRIVER_H_ */
