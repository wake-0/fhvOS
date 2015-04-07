/*
 * systemcallFunctions.h
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#ifndef SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_
#define SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_

#include "../driver/timer/driver_timer.h"
#include "../hal/cpu/hal_cpu.h"
#include "../driver/led/driver_led.h"
#include "../driver/uart/driver_uart.h"
#include "../hal/gpio/hal_gpio.h"


#define SYSTEM_MODE 0x1F
#define USER_MODE 0x10

extern void SystemCallChangeMode(int mode);

#endif /* SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_ */