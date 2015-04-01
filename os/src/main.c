
//#include <stdio.h>

#include "driver/uart/driver_uart.h"
#include "driver/timer/driver_timer.h"
#include "hal/interrupt/hal_interrupt.h"
#include "hal/am335x/hw_cm_dpll.h"
#include "hal/am335x/hw_cm_per.h"
#include "hal/am335x/soc_AM335x.h"
#include "hal/am335x/hw_timer.h"
#include "hal/am335x/hw_types.h"
#include "hal/am335x/hw_cm_dpll.h"

#define DMTIMER_TCLR_AR   (0x00000002u)
#define DMTIMER_TCLR_CE   (0x00000040u)
#define TIMER_INITIAL_COUNT             (0xFF000000u)
#define TIMER_RLD_COUNT                 (0xFF000000u)
#define DMTIMER_IRQSTATUS_RAW_OVF_IT_FLAG   (0x00000002u)
#define DMTIMER_IRQENABLE_SET_OVF_EN_FLAG   (0x00000002u)
#define DMTIMER_TCLR_AR   					(0x00000002u)
#define DMTIMER_AUTORLD_NOCMP_ENABLE        (DMTIMER_TCLR_AR)
#define DMTIMER_INSTANCE                	(DMTIMER2)
#define DMTIMER_INT_OVF_EN_FLAG              (DMTIMER_IRQENABLE_SET_OVF_EN_FLAG)
#define DMTIMER_INT_OVF_IT_FLAG              (DMTIMER_IRQSTATUS_RAW_OVF_IT_FLAG)
#define CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL   (0x00000003u)
#define CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC   (0x1u)
#define CACHE_ICACHE                 (0x01) /* Instruction cache */
#define CACHE_DCACHE                 (0x02) /* Data and Unified cache*/
#define CACHE_ALL                    (0x03) /* Instruction, Data and Unified
                                               Cache at all levels*/
#define DMTIMER_TSICR_POSTED   (0x00000004u)
#define DMTimerWaitForWrite(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\
            while((reg & DMTimerWritePostedStatusGet(baseAdd)));
#define HWREG(address) 						(*((volatile unsigned int *)(address)))
#define TCLR_ST   (0x00000001u)
#define REG_IDX_SHIFT                  (0x05)
#define REG_BIT_MASK                   (0x1F)
#define NUM_INTERRUPTS                 (128u)
/* IRQENABLE_SET */
#define DMTIMER_IRQENABLE_SET_MAT_EN_FLAG   (0x00000001u)
#define DMTIMER_IRQSTATUS_TCAR_IT_FLAG   (0x00000004u)
#define DMTIMER_IRQSTATUS_OVF_IT_FLAG   (0x00000002u)
#define DMTIMER_IRQSTATUS_MAT_IT_FLAG   (0x00000001u)
#define DMTIMER_IRQENABLE_SET_OVF_EN_FLAG   (0x00000002u)
#define DMTIMER_IRQENABLE_SET_TCAR_EN_FLAG   (0x00000004u)
#define DMTIMER_IRQENABLE_CLR_MAT_EN_FLAG   (0x00000001u)
#define DMTIMER_IRQENABLE_CLR_OVF_EN_FLAG   (0x00000002u)
#define TIMER_OVERFLOW                  (0xFFFFFFFFu)
#define TIMER_1MS_COUNT                 (0x5DC0u)
#define DMTIMER_IRQENABLE_CLR_TCAR_EN_FLAG   (0x00000004u)

extern void IntPrioritySet1(unsigned int intrNum, unsigned int priority, unsigned int hostIntRoute);
extern void DMTimerResetConfigure(unsigned int baseAdd, unsigned int rstOption);
extern void DMTimerReset(unsigned int baseAdd);
extern void DMTimerPostedModeConfig(unsigned int baseAdd, unsigned int postMode);
extern unsigned int DMTimerWritePostedStatusGet(unsigned int baseAdd);

extern void AintcInit(void);
void DMTimer2ModuleClkConfig(void);
extern void CPUSwitchToPrivilegedMode(void);
extern void CPUSwitchToUserMode(void);
extern void InterruptHandlerRegister(unsigned int interruptNumber, intHandler_t fnHandler);
extern void InterruptPrioritySet(unsigned int interruptNumber, unsigned int priority);
extern void InterruptHandlerEnable(unsigned int interruptNumber);
extern void TimerCounterValueSet(Timer_t timer, unsigned int value);
extern void TimerReloadValueSet(Timer_t timer, unsigned int countVal);
extern void TimerInterruptStatusClear(Timer_t timer, unsigned int interruptNumber);
extern unsigned int InterruptActiveIrqNumberGet(void);
extern intHandler_t InterruptGetHandler(unsigned int interruptNumber);
void timerISR(void);
interrupt void undef_handler(void);
interrupt void fiq_handler(void);
interrupt void irq_handler(void);

static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;



int main(void)
{
	TimerReset(TIMER2);
	DMTimer2ModuleClkConfig();

	CPUSwitchToUserMode();
	CPUSwitchToPrivilegedMode();

	// ----- INTERRUPT settings -----
	InterruptMasterIRQEnable();

	AintcInit();
	InterruptHandlerRegister(SYS_INT_TINT2, timerISR);
	InterruptPrioritySet(SYS_INT_TINT2, 0x01);
	InterruptHandlerEnable(SYS_INT_TINT2);


	// ----- TIMER settings -----
	unsigned int compareMode = 0x00;		// compare mode disabled
	unsigned int reloadMode = 0x00;			// 0x02 auto-reload

	unsigned int countVal = 0xFF000000;//TIMER_OVERFLOW - (milliSec * TIMER_1MS_COUNT);

	TimerCounterValueSet(TIMER2, countVal);
	TimerReloadValueSet(TIMER2, countVal);
	TimerModeConfigure(TIMER2, compareMode, reloadMode);

	// ----- INTERRUPT enable -----
	TimerInterruptEnable(TIMER2, DMTIMER_INT_OVF_EN_FLAG);

	// ----- TIMER start -----
	TimerStart(TIMER2);


	while(cntValue)
	{
		if(flagIsr == 1)
		{
			//printf("\nOverflow\n");
			cntValue--;
			flagIsr = 0;
		}
	}

	while(1)
	{
	}
}


void timerISR(void)
{
	TimerInterruptDisable(TIMER2, 0x2);		// IRQ_OVERFLOW_ENABLE

	TimerInterruptStatusClear(TIMER2, 0x2); // IRQ_OVERFLOW

	flagIsr = 1;

	//printf("\nTimer Interrupt!\n");

	TimerStop(TIMER2);

	//TimerInterruptEnable(TIMER2, 0x2);		// IRQ_OVERFLOW_ENABLE
}

#pragma INTERRUPT(irq_handler, IRQ)
interrupt void irq_handler()
{
	//InterruptSaveUserContext();

	unsigned int activeIrq = InterruptActiveIrqNumberGet();

	intHandler_t timerIsr = InterruptGetHandler(activeIrq);
	timerIsr();

	//InterruptRestoreUserContext();
}


#pragma INTERRUPT(udef_handler, UDEF)
interrupt void udef_handler() {
	printf("udef interrupt\n");
	;

}

#pragma INTERRUPT(fiq_handler, FIQ)
interrupt void fiq_handler() {
	printf("fiq interrupt\n");
	;
}

#pragma INTERRUPT(pabt_handler, PABT)
interrupt void pabt_handler() {
	printf("pabt interrupt\n");
	;
}


/**
 * Is called on any prefetch abort.
 */
#pragma INTERRUPT(dabt_handler, PABT)
interrupt void dabt_handler() {
	printf("dabt interrupt\n");
}


void DMTimer2ModuleClkConfig(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) =
                             CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) =
                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) =
                             CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) =
                             CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) =
                             CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) !=
                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) =
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) !=
                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) =
                             CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    /* Select the clock source for the Timer2 instance. */
    HWREG(SOC_CM_DPLL_REGS + CM_DPLL_CLKSEL_TIMER2_CLK) &=
          ~(CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL);

    HWREG(SOC_CM_DPLL_REGS + CM_DPLL_CLKSEL_TIMER2_CLK) |=
          CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC;

    while((HWREG(SOC_CM_DPLL_REGS + CM_DPLL_CLKSEL_TIMER2_CLK) &
           CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL) !=
           CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC);

    HWREG(SOC_CM_PER_REGS + CM_PER_TIMER2_CLKCTRL) |=
                             CM_PER_TIMER2_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_TIMER2_CLKCTRL) &
    CM_PER_TIMER2_CLKCTRL_MODULEMODE) != CM_PER_TIMER2_CLKCTRL_MODULEMODE_ENABLE);

    while((HWREG(SOC_CM_PER_REGS + CM_PER_TIMER2_CLKCTRL) &
       CM_PER_TIMER2_CLKCTRL_IDLEST) != CM_PER_TIMER2_CLKCTRL_IDLEST_FUNC);

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK))); // inactive CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER2_GCLK)));

}

static void IntDefaultHandler(void)
{
    /* Go Back. Nothing to be done */
    ;
}






