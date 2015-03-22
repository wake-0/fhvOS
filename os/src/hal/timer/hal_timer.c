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



void startTimer(unsigned int baseRegister)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= DMTIMER_TCLR_START;
	else
		HWREG(baseRegister + TCLR) |= DMTIMER_TCLR_START;
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
void setTimerCounterReloadMode(unsigned int baseRegister, unsigned int reloadMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= reloadMode;
	else
		HWREG(baseRegister + TCLR) |= reloadMode;
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: compareMode				0x00 compare mode disabled
 *									0x20 compare mode enabled
 */
void setTimerCounterCompareMode(unsigned int baseRegister, unsigned int compareMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= compareMode;
	else
		HWREG(baseRegister + TCLR) |= compareMode;
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
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= triggerMode;
	else
		HWREG(baseRegister + TCLR) |= triggerMode;
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: triggerMode				0x0000 single capture
 *									0x4000 capture on second event
 */
void setTimerCounterCaptureMode(unsigned int baseRegister, unsigned int controlRegister, unsigned int captureMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= captureMode;
	else
		HWREG(baseRegister + TCLR) |= captureMode;
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: pinMode					0x0000 pulse
 *									0x1000 toggle
 */
void setOutputPinMode(unsigned int baseRegister, unsigned int controlRegister, unsigned int pinMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= pinMode;
	else
		HWREG(baseRegister + TCLR) |= pinMode;
}

/*
 *	\brief:
 *	\param: baseRegister		base address of timer
 *	\param: transitionMode			0x000 no capture
 *									0x100 capture on low to high transition
 *									0x200 capture on high to low transition
 *									0x300 capture on both edge transition
 */
void setTransitionCaptureMode(unsigned int baseRegister, unsigned int transitionMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, baseRegister);

	if(DMTIMER1_MS == baseRegister)
		HWREG(baseRegister + TCLR_1MS) |= transitionMode;
	else
		HWREG(baseRegister + TCLR) |= transitionMode;
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
