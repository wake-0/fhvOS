/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "hal_timer.h"
#include "../../hal/am335x/hw_cm_per.h"
#include "../../hal/am335x/hw_cm_wkup.h"
#include "../../hal/am335x/soc_AM335x.h"
#include "../../hal/am335x/hw_timer.h"
#include "../../hal/am335x/hw_types.h"
#include "../../hal/am335x/hw_cm_dpll.h"
#include "../interrupt/hal_interrupt.h"

#define DMTimerWaitForWrite(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\
            while((reg & TimerHalGetPostedStatus(baseAdd)));	// When equal to 1, a write is pending to the controlRegister register

#define DMTimerWaitForWrite_1MS(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR_1MS) & DMTIMER_TSICR_POSTED)\
            while((reg & TimerHalGetPostedStatus(baseAdd)));	// When equal to 1, a write is pending to the controlRegister register



static void TimerHalSetControlRegisterField(unsigned int baseRegister, unsigned int controlSettings)
{
	if(DMTIMER1_MS == baseRegister)
	{
		HWREG(baseRegister + TCLR_1MS) |= controlSettings;
	}
	else
	{
		HWREG(baseRegister + TCLR) |= controlSettings;
	}
}

void TimerHalActivateTimerInPowerControl(unsigned int timer)
{

}


void TimerHalEnableInterrupts(unsigned int baseRegister, unsigned int timerIrq)
{
    HWREG(baseRegister + IRQENABLE_SET) = (timerIrq &
	                                           (IRQ_CAPTURE_ENABLE |
											    IRQ_OVERFLOW_ENABLE |
												IRQ_MATCH_ENABLE));
}


void TimerHalDisableInterrupts(unsigned int baseRegister, unsigned int timerIrq)
{
	HWREG(baseRegister + IRQENABLE_CLR) = (timerIrq &
	                                           (IRQ_CAPTURE_ENABLE |
											    IRQ_OVERFLOW_ENABLE |
												IRQ_MATCH_ENABLE));
}


void TimerHalClearInterruptStatus(unsigned int baseRegister, unsigned int timerIrq)
{
    /* Clear the interrupt status from IRQSTATUS register */
    HWREG(baseRegister + IRQSTATUS) = (timerIrq &
                                         (IRQ_CAPTURE_ENABLE |
										 IRQ_OVERFLOW_ENABLE |
										 IRQ_MATCH_ENABLE));
}

void TimerHalWriteIrqStatusRawRegister(unsigned int baseRegister, unsigned int timerIrq)
{
	HWREG(baseRegister + IRQSTATUS_RAW) = (timerIrq &
	                                         (IRQ_CAPTURE_ENABLE |
											 IRQ_OVERFLOW_ENABLE |
											 IRQ_MATCH_ENABLE));
}

void TimerHalStart(unsigned int baseRegister)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);
	TimerHalSetControlRegisterField(baseRegister, DMTIMER_TCLR_START);
}


void TimerHalStop(unsigned int baseRegister)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
	{
		HWREG(baseRegister + TCLR_1MS) &= DMTIMER_TCLR_STOP;
	}
	else
	{
		HWREG(baseRegister + TCLR) &= DMTIMER_TCLR_STOP;
	}
}


void TimerHalSetCounterValue(unsigned int baseRegister, unsigned int counterValue)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
	{
		HWREG(baseRegister + TCRR_1MS) = counterValue;
	}
	else
	{
		HWREG(baseRegister + TCRR) = counterValue;
	}
}

/*
 *	\brief:
 *	\param: 	baseRegister		base address of timer
 *	\return:	actual value of timer
 */
unsigned int TimerHalGetCounterValue(unsigned int baseRegister)
{
	if(DMTIMER1_MS == baseRegister)
	{
		DMTimerWaitForWrite_1MS(WRITE_PEND_TCRR, baseRegister);
		return HWREG(baseRegister + TCRR_1MS);
	}
	else
	{
		DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);
		return HWREG(baseRegister + TCRR);
	}
}


unsigned int TimerHalGetReloadValue(unsigned int baseRegister)
{
	if(DMTIMER1_MS == baseRegister)
	{
		DMTimerWaitForWrite_1MS(WRITE_PEND_TCRR, baseRegister);
		return HWREG(baseRegister + TLDR_1MS);
	}
	else
	{
		DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);
		return HWREG(baseRegister + TLDR);
	}
}

/*
 *	\brief:
 *	\param: 	baseRegister				base address of timer
 *	\param:		timerCounterReloadValue		reload value of specified timer
 */
void TimerHalSetCounterReloadValue(unsigned int baseRegister, unsigned int timerCounterReloadValue)
{
	if(DMTIMER1_MS == baseRegister)
	{
		DMTimerWaitForWrite_1MS(WRITE_PEND_TCRR, baseRegister);
		HWREG(baseRegister + TLDR_1MS) = timerCounterReloadValue;
	}
	else
	{
		DMTimerWaitForWrite(WRITE_PEND_TLDR, baseRegister);
		HWREG(baseRegister + TLDR) = timerCounterReloadValue;
	}
}



void TimerHalResetCounterRegister(unsigned int baseRegister, unsigned int resetValue)
{
	HWREG(baseRegister + TIOCP_CFG) |= TIOCP_CFG_SOFTRESET;

	while(TIOCP_CFG_SOFTRESET == (HWREG(baseRegister + TIOCP_CFG) & TIOCP_CFG_SOFTRESET))
		;
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: reloadMode				0x00 one-shot timer
 *									0x02 auto-reload
 */
static void TimerHalSetReloadMode(unsigned int baseRegister, unsigned int reloadMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);
	unsigned int controlSetting = reloadMode & (ONE_SHOT_TIMER | AUTO_RELOAD);
	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}


/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: mode				0x00 compare mode disabled
*								0x20 compare mode enabled
 */
static void TimerHalSetCompareMode(unsigned int baseRegister, unsigned int compareMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);
	unsigned int controlSetting = compareMode & (ENABLE_COMPARE | DISABLE_COMPARE);
	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}


void TimerHalConfigureMode(unsigned int baseRegister, unsigned int compareMode, unsigned int reloadMode)
{
	TimerHalSetCompareMode(baseRegister, compareMode);
	TimerHalSetReloadMode(baseRegister, reloadMode);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: triggerMode				0x000 no trigger
 *									0x400 trigger on overflow
 *									0x800 trigger on overflow and match
 */
void TimerHalSetTriggerMode(unsigned int baseRegister, unsigned int triggerMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = triggerMode & (NO_TRIGGER | OVERFLOW_TRIGGER
			| OVERFLOW_MATCH_TRIGGER);

	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: captureMode				0x0000 single capture
 *									0x4000 capture on second event
 */
void TimerHalSetCaptureMode(unsigned int baseRegister, unsigned int captureMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);
	unsigned int controlSetting = captureMode & (SINGLE_CAPTURE | CAPTURE_ON_SECOND_EVENT);
	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: pinMode					0x0000 pulse
 *									0x1000 toggle
 */
void TimerHalSetOutputPinMode(unsigned int baseRegister, unsigned int controlRegister, unsigned int pinMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);
	unsigned int controlSetting = pinMode & (PINMODE_PULSE | PINMODE_TOGGLE);
	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister			base address of timer
 *	\param: transitionMode			0x000 no capture
 *									0x100 capture on low to high transition
 *									0x200 capture on high to low transition
 *									0x300 capture on both edge transition
 */
void TimerHalSetTransitionCaptureMode(unsigned int baseRegister, unsigned int transitionMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = transitionMode & (NO_CAPTURE
									| CAPTURE_ON_TRANSITION_LOW_TO_HIGH
									| CAPTURE_ON_TRANSITION_HIGH_TO_LOW
									| CAPTURE_ON_BOTH_EDGE_TRANSITION);

	TimerHalSetControlRegisterField(baseRegister, controlSetting);
}



unsigned int TimerHalGetPostedStatus(unsigned int baseRegister)
{
    return (HWREG(baseRegister + TWPS));
}


void TimerHalClearIrqPendingFlag(unsigned int baseRegister, unsigned int timerIrq)
{
	HWREG(baseRegister + IRQSTATUS) = (timerIrq &
	                                         (IRQ_CAPTURE_ENABLE |
											 IRQ_OVERFLOW_ENABLE |
											 IRQ_MATCH_ENABLE));
}


void TimerHalEnableClockPrescaler(unsigned int baseRegister, unsigned int prescalerValue)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	HWREG(baseRegister + TCLR) |= (prescalerValue & (PRESCALER_ENABLE | PRESCALER_VALUE_RANGE));
}


void TimerHalDisableClockPrescaler(unsigned int baseRegister)
{
    DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

    HWREG(baseRegister + TCLR) &= PRESCALER_DISABLE;
}


static void TimerHalModuleClockConfig(void)
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

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) = CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

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
}

static void TimerHalSetTimerClkSource(unsigned int timerMuxSelectionRegister, unsigned int clkSource)
{
	HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) &=
	          ~(TIMER_CLK_SELECTION_RANGE);

	unsigned int controlSetting = clkSource & (TIMER_CLK_SELECT_TCLKIN | TIMER_CLK_SELECT_M_OSC
																	| TIMER_CLK_SELECT_32KHZ);

	HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) |= controlSetting;

	while((HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) &
			TIMER_CLK_SELECTION_RANGE) != controlSetting);
}

static void TimerHalEnableSelectedClockSource(unsigned int timerClockControlRegister)
{
	HWREG(SOC_CM_PER_REGS + timerClockControlRegister) |= CLK_ENABLE;

	while((HWREG(SOC_CM_PER_REGS + timerClockControlRegister) &
	CLK_MODULEMODE_RANGE) != CLK_ENABLE);
}

static void TimerHalWaitForModuleBeingFullyFunctional(unsigned int timerClockControlRegister)
{
	while((HWREG(SOC_CM_PER_REGS + timerClockControlRegister) &
			CLK_IDLE_STATUS_RANGE) != CLK_IDLE_STATUS_FULLY_FUNCTIONAL);
}

static void TimerHalWaitForL3SClockBeingActive(void)
{
	while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
			CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));
}

static void TimerHalWaitForL3ClockBeingActive(void)
{
	while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
	            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));
}

static void TimerHalWaitForOcpwpClockBeingActive(void)
{
	while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
		   (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
			CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));
}


static void TimerHalActivateClockDomain(unsigned int baseRegister, unsigned int controlRegister, unsigned int settings)
{
	while(!(HWREG(baseRegister + controlRegister) & (settings)));
}

static void TimerHalActivateClock(unsigned int timer)
{
	switch(timer)
	{
		case TIMER0:
			TimerHalActivateClockDomain(SOC_CM_WKUP_REGS, CM_WKUP_CLKSTCTRL, (CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK
					| CM_WKUP_CLKSTCTRL_CLKACTIVITY_TIMER0_GCLK));
			break;
		case TIMER1_MS:
			TimerHalActivateClockDomain(SOC_CM_WKUP_REGS, CM_WKUP_CLKSTCTRL, (CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK
					| CM_WKUP_CLKSTCTRL_CLKACTIVITY_TIMER1_GCLK));
			break;
		case TIMER2:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER2_GCLK));
			break;
		case TIMER3:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER3_GCLK));
			break;
		case TIMER4:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER4_GCLK));
			break;
		case TIMER5:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER5_GCLK));
			break;
		case TIMER6:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER6_GCLK));
			break;
		case TIMER7:
			TimerHalActivateClockDomain(SOC_CM_PER_REGS, CM_PER_L4LS_CLKSTCTRL, (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK
					| CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER7_GCLK));
			break;
	}
}

/*
// TODO: refactor function to activate all timers, not just timer2
static void TimerHalSetTimerClockActive(void)
{
	while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
	           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
	            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER2_GCLK)));
}
*/

void TimerHalSetClockSettings(unsigned int timer, unsigned int timerMuxSelectionRegister, unsigned int timerClockControlRegister, unsigned int clkSource)
{
	TimerHalModuleClockConfig();

    TimerHalSetTimerClkSource(timerMuxSelectionRegister, clkSource);   //TIMER_CLK_SELECT_TCLKIN, TIMER_CLK_SELECT_M_OSC, TIMER_CLK_SELECT_32KHZ

    TimerHalEnableSelectedClockSource(timerClockControlRegister);

    TimerHalWaitForModuleBeingFullyFunctional(timerClockControlRegister);

    TimerHalWaitForL3SClockBeingActive();

    TimerHalWaitForL3ClockBeingActive();

    TimerHalWaitForOcpwpClockBeingActive(); // l4 ocpwp not active

    TimerHalActivateClock(timer);
}


unsigned int TimerHalGetInterruptNumber(unsigned int timer)
{
	switch(timer)
	{
		case TIMER0:
			return SYS_INT_TINT0;
		case TIMER1_MS:
			return SYS_INT_TINT1_1MS;
		case TIMER2:
			return SYS_INT_TINT2;
		case TIMER3:
			return SYS_INT_TINT3;
		case TIMER4:
			return SYS_INT_TINT4;
		case TIMER5:
			return SYS_INT_TINT5;
		case TIMER6:
			return SYS_INT_TINT6;
		case TIMER7:
			return SYS_INT_TINT7;
		default:
			return 0;
	}
}


unsigned int TimerHalGetClockControlRegisterAddress(unsigned int timer)
{
	switch(timer)
	{
		case TIMER0:
			return 0;				// no clock settings for timer0
		case TIMER1_MS:
			return 0;				// CM_PER_TIMER1MS_CLKCTRL
		case TIMER2:
			return CM_PER_TIMER2_CLKCTRL;
		case TIMER3:
			return CM_PER_TIMER3_CLKCTRL;
		case TIMER4:
			return CM_PER_TIMER4_CLKCTRL;
		case TIMER5:
			return CM_PER_TIMER5_CLKCTRL;
		case TIMER6:
			return CM_PER_TIMER6_CLKCTRL;
		case TIMER7:
			return CM_PER_TIMER7_CLKCTRL;
		default:
			return 0;
	}
}

unsigned int TimerHalGetMuxRegisterAddress(unsigned int timer)
{
	switch(timer)
	{
		case TIMER0:
			return 0;				// no clock settings for timer0
		case TIMER1_MS:
			return CM_DPLL_CLKSEL_TIMER1MS_CLK;
		case TIMER2:
			return CM_DPLL_CLKSEL_TIMER2_CLK;
		case TIMER3:
			return CM_DPLL_CLKSEL_TIMER3_CLK;
		case TIMER4:
			return CM_DPLL_CLKSEL_TIMER4_CLK;
		case TIMER5:
			return CM_DPLL_CLKSEL_TIMER5_CLK;
		case TIMER6:
			return CM_DPLL_CLKSEL_TIMER6_CLK;
		case TIMER7:
			return CM_DPLL_CLKSEL_TIMER7_CLK;
		default:
			return 0;
	}
}


/*
 *	\brief:
 *	\param: 	timer		number of used timer
	\return: 	base register address of timer
 */
unsigned int TimerHalGetTimerBaseAddress(unsigned int timer)
{
	switch(timer)
	{
		case 0:
			return DMTIMER0;
		case 1:
			return DMTIMER1_MS;
		case 2:
			return DMTIMER2;
		case 3:
			return DMTIMER3;
		case 4:
			return DMTIMER4;
		case 5:
			return DMTIMER5;
		case 6:
			return DMTIMER6;
		case 7:
			return DMTIMER7;
		default:
			return 0;
	}
}
