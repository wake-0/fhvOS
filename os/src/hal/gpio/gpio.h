/*
 * gpio.h
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef HAL_GPIO_GPIO_H_
#define HAL_GPIO_GPIO_H_

#define bus_number_t			char	// TODO Check if in this context this is called "bus". Should this be "bank"?
#define BUS_NUMBER_UNDEF		-1

#define pin_mode_t				unsigned char
#define PIN_MODE_GPIO			0
#define PIN_MODE_PWMSEL			1
// TODO Add more pin modes

#define pin_direction_t			unsigned char
#define PIN_DIRECTION_IN		1
#define PIN_DIRECTION_OUT		0

#define pin_value_t				unsigned char
#define PIN_VALUE_HIGH			1
#define PIN_VALUE_LOW			0

#define UNDEFINED_PIN			-1

extern void 		GPIOEnable(bus_number_t busNo);
extern void 		GPIODisable(bus_number_t busNo);
extern void 		GPIOReset(bus_number_t busNo);
extern bus_number_t GPIOGetPinBus(int pin);
extern void 		GPIOSetMux(int pin, int mux); // TODO Check if mux should be added to above types
extern void 		GPIOSetMode(int pin, pin_mode_t mode);
extern void 		GPIOSetPinValue(int pin, pin_direction_t dir);
extern pin_value_t 	GPIOGetPinValue(int pin, pin_value_t value);

extern int			GPIOGetLedPin(unsigned int ledNo);
extern int			GPIOGetLedCount();

#endif /* HAL_GPIO_GPIO_H_ */
