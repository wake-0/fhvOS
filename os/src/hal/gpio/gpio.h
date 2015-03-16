/*
 * gpio.h
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef HAL_GPIO_GPIO_H_
#define HAL_GPIO_GPIO_H_

#define pin_mode_t				unsigned char
#define PIN_MODE_GPIO			0
#define PIN_MODE_PWMSEL			1
// TODO Add more pin modes

typedef enum pin_direction_t {
	PIN_DIRECTION_IN = 1,
	PIN_DIRECTION_OUT = 2
} pin_direction_t;

typedef enum pin_value_t {
	PIN_VALUE_HIGH = 1,
	PIN_VALUE_LOW = 2
} pin_value_t;

#define UNDEFINED_PIN			-1

extern void 		GPIOEnable(int pin);
extern void 		GPIODisable(int pin);
extern void 		GPIOReset(int pin);
extern void 		GPIOSetMux(int pin, int mux);
extern void 		GPIOSetPinDirection(int pin, pin_direction_t dir);
extern void 		GPIOSetPinValue(int pin, pin_value_t value);
extern pin_value_t 	GPIOGetPinValue(int pin);

extern int			GPIOGetLedPin(unsigned int ledNo);
extern int			GPIOGetLedCount();

#endif /* HAL_GPIO_GPIO_H_ */
