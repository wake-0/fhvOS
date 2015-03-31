/*
 * gpio.h
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef HAL_GPIO_GPIO_H_
#define HAL_GPIO_GPIO_H_

#include "../../platform/platform.h"

typedef enum {
	PIN_DIRECTION_IN = 1,
	PIN_DIRECTION_OUT = 2
} pin_direction_t;

typedef enum {
	PIN_VALUE_HIGH = 1,
	PIN_VALUE_LOW = 2
} pin_value_t;

typedef enum {
	MUX_MODE_LED = 1
} mux_mode_t;

#define UNDEFINED_PIN			-1

extern void 		GPIOEnable(uint16_t pin);
extern void 		GPIODisable(uint16_t pin);
extern void 		GPIOReset(uint16_t pin);
extern void 		GPIOSetMux(uint16_t pin, mux_mode_t mux);
extern void 		GPIOSetPinDirection(uint16_t pin, pin_direction_t dir);
extern void 		GPIOSetPinValue(uint16_t pin, pin_value_t value);
extern pin_value_t 	GPIOGetPinValue(uint16_t pin);

#endif /* HAL_GPIO_GPIO_H_ */
