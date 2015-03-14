/*
 * uart.c
 *
 *  Created on: 14.03.2015
 *      Author: Kevin
 */
#include "uart.h"
#include "../am335x/soc_AM335x.h"
#include "../../hw_cm_per.h"
#include "../../hw_cm_wkup.h"
#include "../../hw_control_AM335x.h"

#define HWREG(x) (*((volatile unsigned int *)(x)))

//TODO: add other UARTs
void UART0ModuleClkConfig(void) {
	/* Configuring L3 Interface Clocks. */

	/* Writing to MODULEMODE field of CM_PER_L3_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) |=
			CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL)
					& CM_PER_L3_CLKCTRL_MODULEMODE))
		;

	/* Writing to MODULEMODE field of CM_PER_L3_INSTR_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) |=
			CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL)
					& CM_PER_L3_INSTR_CLKCTRL_MODULEMODE))
		;

	/* Writing to CLKTRCTRL field of CM_PER_L3_CLKSTCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) |=
			CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	/* Waiting for CLKTRCTRL field to reflect the written value. */
	while (CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL)
					& CM_PER_L3_CLKSTCTRL_CLKTRCTRL))
		;

	/* Writing to CLKTRCTRL field of CM_PER_OCPWP_L3_CLKSTCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) |=
			CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	/*Waiting for CLKTRCTRL field to reflect the written value. */
	while (CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL)
					& CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL))
		;

	/* Writing to CLKTRCTRL field of CM_PER_L3S_CLKSTCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) |=
			CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	/*Waiting for CLKTRCTRL field to reflect the written value. */
	while (CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL)
					& CM_PER_L3S_CLKSTCTRL_CLKTRCTRL))
		;

	/* Checking fields for necessary values.  */

	/* Waiting for IDLEST field in CM_PER_L3_CLKCTRL register to be set to 0x0. */
	while ((CM_PER_L3_CLKCTRL_IDLEST_FUNC << CM_PER_L3_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL)
					& CM_PER_L3_CLKCTRL_IDLEST))
		;

	/*
	 ** Waiting for IDLEST field in CM_PER_L3_INSTR_CLKCTRL register to attain the
	 ** desired value.
	 */
	while ((CM_PER_L3_INSTR_CLKCTRL_IDLEST_FUNC
			<< CM_PER_L3_INSTR_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL)
					& CM_PER_L3_INSTR_CLKCTRL_IDLEST))
		;

	/*
	 ** Waiting for CLKACTIVITY_L3_GCLK field in CM_PER_L3_CLKSTCTRL register to
	 ** attain the desired value.
	 */
	while (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL)
					& CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK))
		;

	/*
	 ** Waiting for CLKACTIVITY_OCPWP_L3_GCLK field in CM_PER_OCPWP_L3_CLKSTCTRL
	 ** register to attain the desired value.
	 */
	while (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL)
					& CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK))
		;

	/*
	 ** Waiting for CLKACTIVITY_L3S_GCLK field in CM_PER_L3S_CLKSTCTRL register
	 ** to attain the desired value.
	 */
	while (CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL)
					& CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK))
		;

	/* Configuring registers related to Wake-Up region. */

	/* Writing to MODULEMODE field of CM_WKUP_CONTROL_CLKCTRL register. */
	HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL) |=
			CM_WKUP_CONTROL_CLKCTRL_MODULEMODE_ENABLE;

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (CM_WKUP_CONTROL_CLKCTRL_MODULEMODE_ENABLE
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL)
					& CM_WKUP_CONTROL_CLKCTRL_MODULEMODE))
		;

	/* Writing to CLKTRCTRL field of CM_PER_L3S_CLKSTCTRL register. */
	HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) |=
			CM_WKUP_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	/*Waiting for CLKTRCTRL field to reflect the written value. */
	while (CM_WKUP_CLKSTCTRL_CLKTRCTRL_SW_WKUP
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL)
					& CM_WKUP_CLKSTCTRL_CLKTRCTRL))
		;

	/* Writing to CLKTRCTRL field of CM_L3_AON_CLKSTCTRL register. */
	HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L3_AON_CLKSTCTRL) |=
			CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	/*Waiting for CLKTRCTRL field to reflect the written value. */
	while (CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKTRCTRL_SW_WKUP
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L3_AON_CLKSTCTRL)
					& CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKTRCTRL))
		;

	/* Writing to MODULEMODE field of CM_WKUP_UART0_CLKCTRL register. */
	HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) |=
			CM_WKUP_UART0_CLKCTRL_MODULEMODE_ENABLE;

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (CM_WKUP_UART0_CLKCTRL_MODULEMODE_ENABLE
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL)
					& CM_WKUP_UART0_CLKCTRL_MODULEMODE))
		;

	/* Verifying if the other bits are set to required settings. */

	/*
	 ** Waiting for IDLEST field in CM_WKUP_CONTROL_CLKCTRL register to attain
	 ** desired value.
	 */
	while ((CM_WKUP_CONTROL_CLKCTRL_IDLEST_FUNC
			<< CM_WKUP_CONTROL_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL)
					& CM_WKUP_CONTROL_CLKCTRL_IDLEST))
		;

	/*
	 ** Waiting for CLKACTIVITY_L3_AON_GCLK field in CM_L3_AON_CLKSTCTRL
	 ** register to attain desired value.
	 */
	while (CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L3_AON_CLKSTCTRL)
					& CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK))
		;

	/*
	 ** Waiting for IDLEST field in CM_WKUP_L4WKUP_CLKCTRL register to attain
	 ** desired value.
	 */
	while ((CM_WKUP_L4WKUP_CLKCTRL_IDLEST_FUNC
			<< CM_WKUP_L4WKUP_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL)
					& CM_WKUP_L4WKUP_CLKCTRL_IDLEST))
		;

	/*
	 ** Waiting for CLKACTIVITY_L4_WKUP_GCLK field in CM_WKUP_CLKSTCTRL register
	 ** to attain desired value.
	 */
	while (CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL)
					& CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK))
		;

	/*
	 ** Waiting for CLKACTIVITY_L4_WKUP_AON_GCLK field in CM_L4_WKUP_AON_CLKSTCTRL
	 ** register to attain desired value.
	 */
	while (CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL)
					& CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK))
		;

	/*
	 ** Waiting for CLKACTIVITY_UART0_GFCLK field in CM_WKUP_CLKSTCTRL
	 ** register to attain desired value.
	 */
	while (CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL)
					& CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK))
		;

	/*
	 ** Waiting for IDLEST field in CM_WKUP_UART0_CLKCTRL register to attain
	 ** desired value.
	 */
	while ((CM_WKUP_UART0_CLKCTRL_IDLEST_FUNC
			<< CM_WKUP_UART0_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL)
					& CM_WKUP_UART0_CLKCTRL_IDLEST))
		;
}

void UARTPinMuxSetup(unsigned int instanceNum)
{
     if(0 == instanceNum)
     {
          /* RXD */
          HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_RXD(0)) =
          (CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUTYPESEL |
           CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RXACTIVE);

          /* TXD */
          HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_TXD(0)) =
           CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUTYPESEL;
     }

     // TODO: add other instance num
}

