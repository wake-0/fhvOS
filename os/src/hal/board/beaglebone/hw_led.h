/*
 * hw_led.h
 *
 *  Created on: 18.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef HAL_BOARD_BEAGLEBONE_HW_LED_H_
#define HAL_BOARD_BEAGLEBONE_HW_LED_H_

#define	BOARD_LED_COUNT			4
#define	BOARD_LED(x)			(21 + x)

// This is commented intentionally
// The following snippet shows a possible solution to encapsulate the pin->gpio-clock mapping
// We currently assume that this mapping is AM335X-dependant and therefore should be in the
// gpio.c implementation
/*extern int GPIO_CLOCK_FOR_PIN(int pin)
{
	switch(pin)
	{
	case 21:
	case 22:
	case 23:
	case 24:
		return 1;
	default:
		return 0;
	}
}*/


#endif /* HAL_BOARD_BEAGLEBONE_HW_LED_H_ */
