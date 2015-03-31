/*
 * manager.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DRIVER_MANAGER_MANAGER_H_
#define DRIVER_MANAGER_MANAGER_H_

#include "../../../platform/platform.h"
#include "../driver.h"

/*
 * Typedefs
 */
typedef uint16_t driver_id_t;

/*
 * Abstract driver ids
 */
#define DRIVER_ID_LED	123
#define DRIVER_ID_UART	124

extern void DriverManagerInit();
extern driver_t* DriverManagerGetDriver(driver_id_t driver_id);
extern void DriverManagerDestruct();

#endif /* DRIVER_MANAGER_MANAGER_H_ */
