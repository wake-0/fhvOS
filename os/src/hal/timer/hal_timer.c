/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "hal_timer.h"
#include "../../hal/am335x/hw_cm_per.h"
#include "../../hal/am335x/soc_AM335x.h"
#include "../../hal/am335x/hw_timer.h"
#include "../../hal/am335x/hw_types.h"
#include "../../hal/interrupt/interrupt.h"
#include "../../hal/am335x/hw_cm_dpll.h"

#define DMTimerWaitForWrite(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\
            while((reg & DMTimerWritePostedStatusGet(baseAdd)));	// When equal to 1, a write is pending to the controlRegister register



void configureCyclicInterrupt(unsigned int baseRegister, intHandler_t intHandler, unsigned int timeInMilis)
{

}


void setTimerControlRegisterField(unsigned int baseRegister, unsigned int controlSettings)
{
	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= controlSettings;
	else
		HWREG(baseRegister + TCLR) |= controlSettings;
}


void enableTimerInterrupts(unsigned int baseRegister, unsigned int timerIrq)
{
    HWREG(baseRegister + IRQENABLE_CLR) = (timerIrq &
	                                           (IRQ_CAPTURE_ENABLE |
											    IRQ_OVERFLOW_ENABLE |
												IRQ_MATCH_ENABLE));
}


void disableTimerInterrupts(unsigned int baseRegister, unsigned int timerIrq)
{
	HWREG(baseRegister + IRQENABLE_CLR) = (timerIrq &
	                                           (IRQ_CAPTURE_ENABLE |
											    IRQ_OVERFLOW_ENABLE |
												IRQ_MATCH_ENABLE));
}


void startTimer(unsigned int baseRegister)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	setTimerControlRegisterField(baseRegister, DMTIMER_TCLR_START);
}


void stopTimer(unsigned int baseRegister)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) &= DMTIMER_TCLR_STOP;
	else
		HWREG(baseRegister + TCLR) &= DMTIMER_TCLR_STOP;
}


void setTimerCounterValue(unsigned int baseRegister, unsigned int counterValue)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCRR_1MS) = counterValue;
	else
		HWREG(baseRegister + TCRR) = counterValue;
}


/*
 *	\brief:
 *	\param: 	baseRegister		base address of timer
 *	\return:	actual value of timer
 */
unsigned int readTimerCounterValue(unsigned int baseRegister)
{
	if(DMTIMER1_MS == baseRegister)
		return HWREG(baseRegister + TCRR_1MS);
	else
		return HWREG(baseRegister + TCRR);
}

/*
 *	\brief:
 *	\param: 	baseRegister				base address of timer
 *	\param:		timerCounterReloadValue		reload value of specified timer
 */
void setTimerCounterReloadValue(unsigned int baseRegister, unsigned int timerCounterReloadValue)
{
	DMTimerWaitForWrite(WRITE_PEND_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TLDR_1MS) = timerCounterReloadValue;
	else
		HWREG(baseRegister + TLDR) = timerCounterReloadValue;
}



void resetTimerCounterRegister(unsigned int baseRegister, unsigned int resetValue)
{
	HWREG(baseRegister + TIOCP_CFG) |= TIOCP_CFG_SOFTRESET;

	while(TIOCP_CFG_SOFTRESET == (HWREG(baseRegister + TIOCP_CFG) & TIOCP_CFG_SOFTRESET))
		;
}


void configureTimerMode(unsigned int baseRegister, unsigned int compareMode, unsigned int reloadMode)
{
	setTimerCounterCompareMode(baseRegister, compareMode);
	setTimerCounterReloadMode(baseRegister, reloadMode);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: reloadMode				0x00 one-shot timer
 *									0x02 auto-reload
 */
void setTimerCounterReloadMode(unsigned int baseRegister, unsigned int reloadMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = reloadMode & (ONE_SHOT_TIMER | AUTO_RELOAD);

	if(!((controlSetting & ONE_SHOT_TIMER) | (controlSetting & AUTO_RELOAD)))
		return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}


/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: mode				0x00 compare mode disabled
*								0x20 compare mode enabled
 */
void setTimerCounterCompareMode(unsigned int baseRegister, unsigned int compareMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = compareMode & (ENABLE_COMPARE | DISABLE_COMPARE);

	if(!((controlSetting & ENABLE_COMPARE) | (controlSetting & DISABLE_COMPARE)))
			return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: triggerMode				0x000 no trigger
 *									0x400 trigger on overflow
 *									0x800 trigger on overflow and match
 */
void setTimerCounterTriggerMode(unsigned int baseRegister, unsigned int triggerMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = triggerMode & (NO_TRIGGER | OVERFLOW_TRIGGER
			| OVERFLOW_MATCH_TRIGGER);

	if(!((controlSetting & NO_TRIGGER) | (controlSetting & OVERFLOW_TRIGGER)
			| (controlSetting & OVERFLOW_MATCH_TRIGGER)))
				return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: captureMode				0x0000 single capture
 *									0x4000 capture on second event
 */
void setTimerCounterCaptureMode(unsigned int baseRegister, unsigned int captureMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = captureMode & (SINGLE_CAPTURE | CAPTURE_ON_SECOND_EVENT);

	if(!((controlSetting & SINGLE_CAPTURE) | (controlSetting & CAPTURE_ON_SECOND_EVENT)))
		return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: pinMode					0x0000 pulse
 *									0x1000 toggle
 */
void setOutputPinMode(unsigned int baseRegister, unsigned int controlRegister, unsigned int pinMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = pinMode & (PINMODE_PULSE | PINMODE_TOGGLE);

	if(!((controlSetting & PINMODE_PULSE) | (controlSetting & PINMODE_TOGGLE)))
		return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister			base address of timer
 *	\param: transitionMode			0x000 no capture
 *									0x100 capture on low to high transition
 *									0x200 capture on high to low transition
 *									0x300 capture on both edge transition
 */
void setTransitionCaptureMode(unsigned int baseRegister, unsigned int transitionMode)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	unsigned int controlSetting = transitionMode & (NO_CAPTURE
									| CAPTURE_ON_TRANSITION_LOW_TO_HIGH
									| CAPTURE_ON_TRANSITION_HIGH_TO_LOW
									| CAPTURE_ON_BOTH_EDGE_TRANSITION);

	if(!((controlSetting & NO_CAPTURE) | (controlSetting & CAPTURE_ON_TRANSITION_LOW_TO_HIGH)
			| (controlSetting & CAPTURE_ON_TRANSITION_HIGH_TO_LOW)
			| (controlSetting & CAPTURE_ON_BOTH_EDGE_TRANSITION)))
		return;

	setTimerControlRegisterField(baseRegister, controlSetting);
}



unsigned int DMTimerWritePostedStatusGet(unsigned int baseRegister)
{
    return (HWREG(baseRegister + TWPS));
}


void enableClockPrescaler(unsigned int baseRegister, unsigned int prescalerValue)
{
	DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

	HWREG(baseRegister + TCLR) |= (prescalerValue & (PRESCALER_ENABLE | PRESCALER_VALUE_RANGE));
}


void disableClockPrescaler(unsigned int baseRegister)
{
    DMTimerWaitForWrite(WRITE_PEND_TCLR, baseRegister);

    HWREG(baseRegister + TCLR) &= PRESCALER_DISABLE;
}


static void moduleClockConfig(void)
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


void selectClockSourceForTimer(unsigned int timerMuxSelectionRegister, unsigned int timerClockControlRegister)
{
	moduleClockConfig();

    // Select the clock source for the Timer2 instance.
    HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) &=
          ~(CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL);

    HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) |=
          CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC;

    while((HWREG(SOC_CM_DPLL_REGS + timerMuxSelectionRegister) &
           CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL) !=
           CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC);

    HWREG(SOC_CM_PER_REGS + timerClockControlRegister) |=
                             CM_PER_TIMER2_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + timerClockControlRegister) &
    CM_PER_TIMER2_CLKCTRL_MODULEMODE) != CM_PER_TIMER2_CLKCTRL_MODULEMODE_ENABLE);

    while((HWREG(SOC_CM_PER_REGS + timerClockControlRegister) &
       CM_PER_TIMER2_CLKCTRL_IDLEST) != CM_PER_TIMER2_CLKCTRL_IDLEST_FUNC);

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_TIMER2_GCLK)));
}

unsigned int getTimerMuxSelectionRegisterAddress(Timer_t timer)
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
	}
}


/*
 *	\brief:
 *	\param: 	timer		number of used timer
	\return: 	base register address of timer
 */
unsigned int getTimerBaseRegisterAddress(Timer_t timer)
{
	switch(timer)
	{
		case TIMER0:
			return DMTIMER0;
		case TIMER1_MS:
			return DMTIMER1_MS;
			break;
		case TIMER2:
			return DMTIMER2;
		case TIMER3:
			return DMTIMER3;
		case TIMER4:
			return DMTIMER4;
		case TIMER5:
			return DMTIMER5;
		case TIMER6:
			return DMTIMER6;
		case TIMER7:
			return DMTIMER7;
	}
}
