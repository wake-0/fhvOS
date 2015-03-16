#include "hal/am335x/hw_cm_per.h"
#include "hal/am335x/hw_cm_wkup.h"
#include "hal/interrupt/interrupt.h"
#include "hal/am335x/soc_AM335x.h"
#include "hal/uart/hw_uart.h"
#include "hal/uart/uart.h"

#define SOC_PRCM_REGS          					 (0x44E00000)
#define SOC_CM_PER_REGS 	   					 (SOC_PRCM_REGS + 0)
#define CM_PER_GPIO1_CLKCTRL   					 (0xac)
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE   (0x2u)
#define CONTROL_CONF_MUXMODE(n)        			 (n)
#define SOC_CONTROL_REGS                     	 (0x44E10000)
#define SOC_GPIO_1_REGS                      	 (0x4804C000)
#define GPIO_CTRL   							 (0x130)
#define GPIO_CTRL_DISABLEMODULE   			     (0x00000001u)
#define GPIO_SYSCONFIG   						 (0x10)
#define GPIO_SYSCONFIG_SOFTRESET                 (0x00000002u)
#define GPIO_SYSSTATUS   						 (0x114)
#define GPIO_SYSSTATUS_RESETDONE   				 (0x00000001u)

#define GPIO_OE   								 (0x134)
#define GPIO_CLEARDATAOUT   					 (0x190)
#define GPIO_SETDATAOUT   						 (0x194)

#define CM_PER_L4LS_CLKSTCTRL   						(0x0)
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE   		 		(0x00000003u)
#define CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK   	(0x00040000u)
#define CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC   				(0x0u)
#define CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT   			(0x00000010u)
#define CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC   				(0x0u)
#define CM_PER_GPIO1_CLKCTRL_IDLEST_IDLE   				(0x2u)
#define CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK (0x00080000u)
#define CM_PER_GPIO1_CLKCTRL_IDLEST  					(0x00030000u)

// uart
#define CM_PER_OCPWP_L3_CLKSTCTRL   			(0x12c)
#define CM_PER_OCPWP_CLKCTRL   					(0x130)
#define SOC_CM_WKUP_REGS                     	(SOC_PRCM_REGS + 0x400)

/* CONF_UART0_RXD */
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_MMODE   (0x00000007u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_MMODE_SHIFT   (0x00000000u)

#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUDEN   (0x00000008u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUDEN_SHIFT   (0x00000003u)

#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUTYPESEL   (0x00000010u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUTYPESEL_SHIFT   (0x00000004u)

#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RSVD   (0x000FFF80u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RSVD_SHIFT   (0x00000007u)

#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RXACTIVE   (0x00000020u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RXACTIVE_SHIFT   (0x00000005u)

#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_SLEWCTRL   (0x00000040u)
#define CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_SLEWCTRL_SHIFT   (0x00000006u)

/* CONF_UART0_TXD */
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_MMODE   (0x00000007u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_MMODE_SHIFT   (0x00000000u)

#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUDEN   (0x00000008u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUDEN_SHIFT   (0x00000003u)

#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUTYPESEL   (0x00000010u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUTYPESEL_SHIFT   (0x00000004u)

#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_RSVD   (0x000FFF80u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_RSVD_SHIFT   (0x00000007u)

#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_RXACTIVE   (0x00000020u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_RXACTIVE_SHIFT   (0x00000005u)

#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_SLEWCTRL   (0x00000040u)
#define CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_SLEWCTRL_SHIFT   (0x00000006u)

#define SOC_UART_0_REGS                      (0x44E09000)

// PRCM: Power Reset Control Management
// OPTFCLKEN: Optional Function CLK (used to configure the clk frequenz via software)
// GDBCLK: Global debounce clock (this clock is between the PRCM and the GPIO)
// GPMC: General Purpose Memory Controller

#define BAUD_RATE_115200          (115200)
#define UART_MODULE_INPUT_CLK     (48000000)

#define HWREG(x) (*((volatile unsigned int *)(x)))
#define CONTROL_CONF_GPMC_A(n)   (0x840 + (n * 4))

//Uart defines
#define CONTROL_CONF_UART_RXD(n)   (0x970 + ((n) * 0x10))
#define CONTROL_CONF_UART_TXD(n)   (0x974 + ((n) * 0x10))

void UART0ModuleClkConfig(void);
void GPIO1ModuleClkConfig(void);
void resetGPIOModule(void);


void blinkLed(void);
void initLed(void);

int main(void) {

	while(1)
	{}
}

void initLed(void) {

	//-----------------------------------------------------//
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	// set mux mode for a gpio
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(7)) = CONTROL_CONF_MUXMODE(
			7);

	/* Clearing the DISABLEMODULE bit in the Control(CTRL) register. */
	HWREG(SOC_GPIO_1_REGS + GPIO_CTRL) &= ~(GPIO_CTRL_DISABLEMODULE);

	resetGPIOModule();

	HWREG(SOC_GPIO_1_REGS + GPIO_OE) &= ~(1 << 23);

}

void blinkLed(void) {
	HWREG(SOC_GPIO_1_REGS + GPIO_SETDATAOUT) = (1 << 23);

	volatile unsigned int i = 0;
	for (i = 0; i < 0x3FFFF; i++) {
	}

	HWREG(SOC_GPIO_1_REGS + GPIO_CLEARDATAOUT) = (1 << 23);

	for (i = 0; i < 0x3FFFF; i++) {
	}
}

void resetGPIOModule(void) {
	/* Setting the SOFTRESET bit in System Configuration register.
	 Doing so would reset the GPIO module.
	 */
	HWREG(SOC_GPIO_1_REGS + GPIO_SYSCONFIG) |= (GPIO_SYSCONFIG_SOFTRESET);

	/* Waiting until the GPIO Module is reset.*/
	while (!(HWREG(SOC_GPIO_1_REGS + GPIO_SYSSTATUS) & GPIO_SYSSTATUS_RESETDONE))
		;
}

void GPIO1ModuleClkConfig(void) {
	/* enable clock */
	/* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
	CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

	/* Waiting for MODULEMODE field to reflect the written value. */
	while (CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL)
					& CM_PER_GPIO1_CLKCTRL_MODULEMODE))
		;

	/* debounce the clock */
	/* Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL register. */
	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
	CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;

	/* Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired value. */
	while (CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL)
					& CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK))
		;

	/* wait for clock is idle */
	/* Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the desired value. */
	while ((CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC
			<< CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT)
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL)
					& CM_PER_GPIO1_CLKCTRL_IDLEST))
		;

	/* wait for debounced clock is idle /
	 /* Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL register to attain desired value. */
	while (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK
			!= (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL)
					& CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK))
		;
}
