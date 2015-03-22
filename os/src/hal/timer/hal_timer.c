/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "hal_timer.h"

#include "../../hal/am335x/hw_timer.h"
#include "../../hal/am335x/hw_types.h"

#define DMTimerWaitForWrite(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\
            while((reg & DMTimerWritePostedStatusGet(baseAdd)));	// When equal to 1, a write is pending to the controlRegister register

#define enableCompare1 0x00






void setTimerControlRegisterField(unsigned int baseRegister, unsigned int controlSettings)
{
	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= controlSettings;
	else
		HWREG(baseRegister + TCLR) |= controlSettings;
}

void setTimerSettings(unsigned int baseRegister, unsigned int timerSettings)
{

}

void startTimer(unsigned int baseRegister)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, baseRegister);

	setTimerControlRegisterField(baseRegister, DMTIMER_TCLR_START);
}

void stopTimer(unsigned int baseRegister)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) &= DMTIMER_TCLR_STOP;
	else
		HWREG(baseRegister + TCLR) &= DMTIMER_TCLR_STOP;
}

void setTimerCounterValue(unsigned int baseRegister, unsigned int counterValue)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, baseRegister);

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
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, baseRegister);

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
	setTimerCounterReloadMode(baseRegister, reloadMode);
	setTimerCounterCompareMode(baseRegister, compareMode);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: reloadMode				0x00 one-shot timer
 *									0x02 auto-reload
 */
void setTimerCounterReloadMode(unsigned int baseRegister, timerReloadMode_t reloadMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(reloadMode)
	{
		case oneShotTimer:
			controlSetting = DMTIMER_TCLR_ONE_SHOT_TIMER;
			break;
		case autoReload:
			controlSetting = DMTIMER_TCLR_AUTO_RELOAD;
			break;
	}

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: mode				0x00 compare mode disabled
 *									0x20 compare mode enabled
 */
void setTimerCounterCompareMode(unsigned int baseRegister, timerCompareMode_t compareMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(compareMode)
	{
		case enableCompare:
			controlSetting = DMTIMER_TCLR_ENABLE_COMPARE;
			break;
		case disableCompare:
			controlSetting = DMTIMER_TCLR_DISABLE_COMPARE;
			break;
	}

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: triggerMode				0x000 no trigger
 *									0x400 trigger on overflow
 *									0x800 trigger on overflow and match
 */
void setTimerCounterTriggerMode(unsigned int baseRegister, timerTriggerMode_t triggerMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(triggerMode)
	{
		case noTrigger:
			controlSetting = DMTIMER_TCLR_NO_TRIGGER;
			break;
		case overflowTrigger:
			controlSetting = DMTIMER_TCLR_OVERFLOW_TRIGGER;
			break;
		case overflowAndMatchTrigger:
			controlSetting = DMTIMER_TCLR_OVERFLOW_MATCH_TRIGGER;
			break;
	}

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: captureMode				0x0000 single capture
 *									0x4000 capture on second event
 */
void setTimerCounterCaptureMode(unsigned int baseRegister, timerCaptureMode_t captureMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(captureMode)
	{
		case singleCapture:
			controlSetting = DMTIMER_TCLR_SINGLE_CAPTURE;
			break;
		case onSecondEvent:
			controlSetting = DMTIMER_TCLR_CAPTURE_ON_SECOND_EVENT;
			break;
	}

	setTimerControlRegisterField(baseRegister, controlSetting);
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: pinMode					0x0000 pulse
 *									0x1000 toggle
 */
void setOutputPinMode(unsigned int baseRegister, unsigned int controlRegister, timerPinMode_t pinMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(pinMode)
	{
		case pulse:
			controlSetting = DMTIMER_TCLR_PINMODE_PULSE;
			break;
		case toggle:
			controlSetting = DMTIMER_TCLR_PINMODE_TOGGLE;
			break;
	}

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
void setTransitionCaptureMode(unsigned int baseRegister, timerTransitionMode_t transitionMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	unsigned int controlSetting = 0;

	switch(transitionMode)
	{
		case noCapture:
			controlSetting = DMTIMER_TCLR_NO_CAPTURE;
			break;
		case lowToHigh:
			controlSetting = DMTIMER_TCLR_CAPTURE_ON_TRANSITION_LOW_TO_HIGH;
			break;
		case highToLow:
			controlSetting = DMTIMER_TCLR_CAPTURE_ON_TRANSITION_HIGH_TO_LOW;
			break;
		case bothEdges:
			controlSetting = DMTIMER_TCLR_CAPTURE_ON_BOTH_EDGE_TRANSITION;
			break;
	}

	setTimerControlRegisterField(baseRegister, controlSetting);
}



unsigned int DMTimerWritePostedStatusGet(unsigned int baseAdd)
{
    return (HWREG(baseAdd + TWPS));
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
			//return DMTIMER1_MS;
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
