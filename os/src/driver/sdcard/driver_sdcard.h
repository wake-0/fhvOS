/*
 * driver_sdcard.h
 *
 *  Created on: 20.05.2015
 *      Author: Kevin
 */

#ifndef DRIVER_SDCARD_DRIVER_SDCARD_H_
#define DRIVER_SDCARD_DRIVER_SDCARD_H_

#include "../driver.h"

extern int SDCardInit	(uint16_t id);
extern int SDCardOpen	(uint16_t id);
extern int SDCardClose	(uint16_t id);
extern int SDCardWrite	(uint16_t id, char* buf, uint16_t len);
extern int SDCardRead	(uint16_t id, char* buf, uint16_t len);
extern int SDCardIoctl	(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len);

#endif /* DRIVER_SDCARD_DRIVER_SDCARD_H_ */
