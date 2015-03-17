/*
 * gpio.c
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#include "gpio.h"

// Platform dependant includes
#include "../am335x/soc_AM335x.h"
#include "../am335x/hw_gpio_v2.h"
#include "../am335x/hw_cm_per.h"
#include "../am335x/hw_types.h"
#include "../am335x/hw_control_AM335x.h"

#include "../platform/platform.h"

#define PIN_COUNT		(23 * 4)
#define GPIO_COUNT		4
#define LED_COUNT		4

/*
 * Mapping as follows:
 *
 * API Pin#			|		BeagleBone Pin#			|		BeagleBone GPIO#
 */
static int pinMapping[PIN_COUNT][3] = {
		/* LEDS are mapped from 0-4 */
		{ 0, 21, 1},
		{ 1, 22, 1},
		{ 2, 23, 1},
		{ 3, 24, 1},
};
#define API_PIN_NO					0
#define	BEAGLE_BONE_PIN_NO			1
#define BEAGLE_BONE_GPIO_NO			2

static tBoolean gpioXEnabled[GPIO_COUNT];

static int getCM_PER_GPIOx_CLKCTRL(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(int pinNo);
static int getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(int pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST(int pinNo);
static int getSOC_GPIO_x_REGS(int pinNo);

extern void GPIOEnable(int pinNo)
{
	if (gpioXEnabled[pinMapping[pinNo][BEAGLE_BONE_GPIO_NO]]) return;

	// Enable the GPIO Clock
	/* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo)) |=
			getCM_PER_GPIOx_CLKCTRL_MODULEMODE(pinNo);

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo))
					& getCM_PER_GPIOx_CLKCTRL_MODULEMODE(pinNo)))
		;

	/* Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo)) |=
			getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo);

	/* Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired value. */
	while (getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo))
					& getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo)))
		;

	/* Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the desired value. */
	while ((getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(pinNo)
			<< getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(pinNo))
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL)
					& getCM_PER_GPIOx_CLKCTRL_IDLEST(pinNo)))
		;

	/* Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL register to attain desired value. */
	while (getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL)
					& getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(pinNo)))
		;


	// Enable the Module
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_CTRL) &= ~(GPIO_CTRL_DISABLEMODULE);

	gpioXEnabled[pinMapping[pinNo][BEAGLE_BONE_GPIO_NO]] = true;
}

extern void GPIODisable(int pinNo)
{
	if (!gpioXEnabled[pinMapping[pinNo][BEAGLE_BONE_GPIO_NO]]) return;
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_CTRL) |= (GPIO_CTRL_DISABLEMODULE);
}

extern void GPIOReset(int pinNo)
{
	/* Setting the SOFTRESET bit in System Configuration register.
	 Doing so would reset the GPIO module.
	 */
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_SYSCONFIG) |= (GPIO_SYSCONFIG_SOFTRESET);

	/* Waiting until the GPIO Module is reset.*/
	while (!(HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_SYSSTATUS) & GPIO_SYSSTATUS_RESETDONE))
		;
}

extern void GPIOSetMux(int pin, mux_mode_t mux)
{
	int muxMode = -1;
	switch (mux) {
		case MUX_MODE_LED:
			muxMode = 7;
			break;
		default:
			break;
	}
	if (muxMode == -1) return; // Invalid mux mode for beaglebone

	int gpio = pinMapping[pin][BEAGLE_BONE_GPIO_NO];
	switch (gpio) {
		case 1:
			HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(pinMapping[pin][BEAGLE_BONE_PIN_NO])) =
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_SLEWCTRL | 	/* Slew rate slow */
				CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE |	/* Receiver enabled */
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUDEN & (~CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUDEN)) | /* PU_PD enabled */
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUTYPESEL & (~CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUTYPESEL)) | /* PD */
				(CONTROL_CONF_MUXMODE(muxMode))	/* Select mode 7 */
				);
			break;
		// TODO Implement for other GPIO#
		default:
			break;
	}
}

extern void GPIOSetPinDirection(int pin, pin_direction_t dir)
{

    if(dir == PIN_DIRECTION_OUT)
    {
        HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_OE) &= ~(1 << pinMapping[pin][BEAGLE_BONE_PIN_NO]);
    }
    else if(dir == PIN_DIRECTION_IN)
    {
        HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_OE) |= (1 << pinMapping[pin][BEAGLE_BONE_PIN_NO]);
    }
}

extern void GPIOSetPinValue(int pin, pin_value_t dir)
{
    if(dir == PIN_VALUE_HIGH)
    {
        HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_SETDATAOUT) = (1 << pinMapping[pin][BEAGLE_BONE_PIN_NO]);
    }
    else if (dir == PIN_VALUE_LOW)
    {
        HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_CLEARDATAOUT) = (1 << pinMapping[pin][BEAGLE_BONE_PIN_NO]);
    }
}

extern pin_value_t GPIOGetPinValue(int pin)
{
	// TODO Implement this
	return PIN_VALUE_LOW;
}

extern int GPIOGetLedPin(unsigned int ledNo)
{
	if (ledNo > LED_COUNT - 1) return UNDEFINED_PIN;
	return ledNo;
}
extern int GPIOGetLedCount()
{
	return LED_COUNT;
}

static int getCM_PER_GPIOx_CLKCTRL(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_MODULEMODE;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
		default:
			return -1;
	}
}

static int getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;
		default:
			return -1;
	}
}

static int getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT;
		default:
			return -1;
	}
}
static int getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_IDLEST(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return CM_PER_GPIO1_CLKCTRL_IDLEST;
		default:
			return -1;
	}
}

static int getSOC_GPIO_x_REGS(int pinNo)
{
	switch (pinNo) {
		case 0:
		case 1:
		case 2:
		case 3:
			return SOC_GPIO_0_REGS;
		default:
			return -1;
	}
}
