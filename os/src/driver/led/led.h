/*
 * led.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DRIVER_LED_LED_H_
#define DRIVER_LED_LED_H_

#include "../driver.h"

extern int LEDInit	(short id);
extern int LEDOpen	(short id);
extern int LEDClose	(short id);
extern int LEDWrite	(short id, char* buf, int len);
extern int LEDRead	(short id, char* buf, int* len);
extern int LEDIoctl	(short id, int cmd, int mode, char* buf, int len);

#endif /* DRIVER_LED_LED_H_ */
