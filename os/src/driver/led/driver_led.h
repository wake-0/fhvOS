/*
 * led.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DRIVER_LED_DRIVER_LED_H_
#define DRIVER_LED_DRIVER_LED_H_

#include "../driver.h"

extern int LEDInit	(uint16_t id);
extern int LEDOpen	(uint16_t id);
extern int LEDClose	(uint16_t id);
extern int LEDWrite	(uint16_t id, char* buf, uint16_t len);
extern int LEDRead	(uint16_t id, char* buf, uint16_t len);
extern int LEDIoctl	(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len);

#endif /* DRIVER_LED_DRIVER_LED_H_ */
