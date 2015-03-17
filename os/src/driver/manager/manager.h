/*
 * manager.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DRIVER_MANAGER_MANAGER_H_
#define DRIVER_MANAGER_MANAGER_H_

#include "../driver.h"

#define driver_id_t		unsigned short

#define DRIVER_ID_LED	123

extern void init();
extern driver_t* DriverManagerGetDriver(driver_id_t driver_id);

#endif /* DRIVER_MANAGER_MANAGER_H_ */
