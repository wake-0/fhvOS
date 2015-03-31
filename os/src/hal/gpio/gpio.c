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

#include "../../platform/platform.h"

#define GPIO_COUNT		4

static boolean_t gpioXEnabled[GPIO_COUNT];

static int getGPIOFromPin(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(uint16_t pinNo);
static int getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(uint16_t pinNo);
static int getCM_PER_GPIOx_CLKCTRL_IDLEST(uint16_t pinNo);
static int getSOC_GPIO_x_REGS(uint16_t pinNo);

void GPIOEnable(uint16_t pinNo)
{
	if (gpioXEnabled[getGPIOFromPin(pinNo)]) { return; }

	// Enable the GPIO Clock
	// Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register.
	HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo)) |=
			getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(pinNo);

	// Waiting for MODULEMODE field to reflect the written value.
	while (getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo))
					& getCM_PER_GPIOx_CLKCTRL_MODULEMODE(pinNo)))
	{ }

	// Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL register.
	HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo)) |=
			getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo);

	// Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired value.
	while (getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + getCM_PER_GPIOx_CLKCTRL(pinNo))
					& getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(pinNo)))
	{ }

	// Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the desired value.
	while ((getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(pinNo)
			<< getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(pinNo))
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL)
					& getCM_PER_GPIOx_CLKCTRL_IDLEST(pinNo)))
	{ }

	// Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL register to attain desired value.
	while (getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(pinNo)
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL)
					& getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(pinNo)))
	{ }


	// Enable the Module
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_CTRL) &= ~(GPIO_CTRL_DISABLEMODULE);

	gpioXEnabled[getGPIOFromPin(pinNo)] = true;
}

void GPIODisable(uint16_t pinNo)
{
	if (!gpioXEnabled[getGPIOFromPin(pinNo)]) { return; }
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_CTRL) |= (GPIO_CTRL_DISABLEMODULE);
}

void GPIOReset(uint16_t pinNo)
{
	// Setting the SOFTRESET bit in System Configuration register.
	// Doing so would reset the GPIO module.
	HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_SYSCONFIG) |= (GPIO_SYSCONFIG_SOFTRESET);

	// Waiting until the GPIO Module is reset.
	while (!(HWREG(getSOC_GPIO_x_REGS(pinNo) + GPIO_SYSSTATUS) & GPIO_SYSSTATUS_RESETDONE))
	{ }
}

void GPIOSetMux(uint16_t pin, mux_mode_t mux)
{
	int muxMode = -1;
	switch (mux) {
		case MUX_MODE_LED:
			muxMode = 7;
			break;
		default:
			break;
	}
	if (muxMode == -1) { return; } // Invalid mux mode for beaglebone

	int gpio = getGPIOFromPin(pin);
	switch (gpio) {
		case 1:
			HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(pin)) =
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_SLEWCTRL | 	// Slew rate slow
				CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE |	// Receiver enabled
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUDEN & (~CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUDEN)) | // PU_PD enabled
				(CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUTYPESEL & (~CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_PUTYPESEL)) | // PD
				(CONTROL_CONF_MUXMODE(muxMode))	/* Select mode 7 */
				);
			break;
		// TODO Implement for other GPIO#
		default:
			break;
	}
}

void GPIOSetPinDirection(uint16_t pin, pin_direction_t dir)
{

    switch(dir)
    {
    	case PIN_DIRECTION_OUT:
			HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_OE) &= ~(1 << pin);
			break;
    	case PIN_DIRECTION_IN:
			HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_OE) |= (1 << pin);
    		break;
    	default:
    		break;
    }
}

void GPIOSetPinValue(uint16_t pin, pin_value_t dir)
{
	switch (dir)
	{
		case PIN_VALUE_HIGH:
			HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_SETDATAOUT) = (1 << pin);
			break;
		case PIN_VALUE_LOW:
			HWREG(getSOC_GPIO_x_REGS(pin) + GPIO_CLEARDATAOUT) = (1 << pin);
			break;
		default:
			break;
	}
}

pin_value_t GPIOGetPinValue(uint16_t pin)
{
	// TODO Implement this
	return PIN_VALUE_LOW;
}

static int getGPIOFromPin(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return 1;
		default:
			return 0;
	}
}

static int getCM_PER_GPIOx_CLKCTRL(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_MODULEMODE;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_MODULEMODE_ENABLE(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
		default:
			return -1;
	}
}

static int getCM_PER_GPIOx_CLKCTRL_OPTFCLKEN_GPIO_x_GDBCLK(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;
		default:
			return -1;
	}
}

static int getCM_PER_GPIOx_CLKCTRL_IDLEST_FUNC(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_IDLEST_SHIFT(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT;
		default:
			return -1;
	}
}
static int getCM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_x_GDBCLK(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK;
		default:
			return -1;
	}
}
static int getCM_PER_GPIOx_CLKCTRL_IDLEST(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return CM_PER_GPIO1_CLKCTRL_IDLEST;
		default:
			return -1;
	}
}

static int getSOC_GPIO_x_REGS(uint16_t pinNo)
{
	switch (pinNo) {
		case 21:
		case 22:
		case 23:
		case 24:
			return SOC_GPIO_1_REGS;
		default:
			return -1;
	}
}
