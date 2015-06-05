/*
 * driver_dmx.h
 *
 *  Created on: 05.06.2015
 *      Author: Kevin
 */

#ifndef DRIVER_DMX_DRIVER_DMX_H_
#define DRIVER_DMX_DRIVER_DMX_H_

#include "../driver.h"

extern int DMXInit	(uint16_t id);
extern int DMXOpen	(uint16_t id);
extern int DMXClose	(uint16_t id);
extern int DMXWrite	(uint16_t id, char* buf, uint16_t len);
extern int DMXRead	(uint16_t id, char* buf, uint16_t len);
extern int DMXIoctl	(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len);

#endif /* DRIVER_DMX_DRIVER_DMX_H_ */
