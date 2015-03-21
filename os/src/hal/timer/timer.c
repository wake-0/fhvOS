/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "../../hal/am335x/hw_timer.h"
#include "../timer/timer.h"
#include "../../hal/am335x/hw_types.h"
 /*
#define DMTimerWaitForWrite(reg, baseAdd)   \
            if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\
            while((reg & DMTimerWritePostedStatusGet(baseAdd)));	// When equal to 1, a write is pending to the TCLR register
*/

// private functions
static unsigned int getTimerBaseRegisterAddress(Timer_t timer);
static void resetTimerCounterRegister(unsigned int timerBaseRegister, unsigned int resetValue);
static void loadTimerValue(unsigned int timerBaseRegister, unsigned int timerCounterValue);
static void DMTimerWaitForWrite(reg, baseAdd);
static void setTimerCounterReloadMode(unsigned int timerBaseRegister, unsigned int reloadMode);
static void setTimerCounterCompareMode(unsigned int timerBaseRegister, unsigned int compareMode);

void TimerEnable(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	DMTimerWaitForWrite(DMTIMER_TWPS_W_PEND_TCLR, timerBaseRegister);
}

void disableTimer(Timer_t timer)
{

}

void TimerReset(Timer_t timer)
{

}

void TimerModeConfigure(Timer_t timer, unsigned int mode)
{

}

void TimerStart(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	HWREG(timerBaseRegister + TCLR) |= DMTIMER_TCLR_START;
}

void TimerStop(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	HWREG(timerBaseRegister + TCLR) &= DMTIMER_TCLR_STOP;
}

void TimerCountingHalt(Timer_t timer)
{

}

void TimerCountingResume(Timer_t timer)
{

}

void TimerValueLoad(Timer_t timer, unsigned int timerCounterValue)
{

}

static void setTimerCounterValue(unsigned int timerBaseRegister, unsigned int timerCounterValue)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, timerBaseRegister);

	HWREG(timerBaseRegister + TCRR) = timerCounterValue;
}

/*
 *	\brief:
 *	\param: 	timerBaseRegister		base address of timer
 *	\return:	actual value of timer
 */
static unsigned int readTimerCounterValue(unsigned int timerBaseRegister)
{
	return HWREG(timerBaseRegister + TCRR);
}

/*
 *	\brief:
 *	\param: 	timerBaseRegister			base address of timer
 *	\param:		timerCounterReloadValue:	reload value of specified timer
 */
static void setTimerCounterReloadValue(unsigned int timerBaseRegister, unsigned int timerCounterReloadValue)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCRR, timerBaseRegister);

	HWREG(timerBaseRegister + TLDR) = timerCounterReloadValue;
}



static void resetTimerCounterRegister(unsigned int timerBaseRegister, unsigned int resetValue)
{

}

static void configureTimerMode(unsigned int timerBaseRegister, unsigned int compareMode, unsigned int reloadMode)
{
	setTimerCounterReloadMode(timerBaseRegister, reloadMode);
	setTimerCounterCompareMode(timerBaseRegister, compareMode);
}

/*
 *	\brief:
 *	\param: timerBaseRegister		base address of timer
 *	\param: reloadMode				0x00 one-shot timer
 *									0x02 auto-reload
 */
static void setTimerCounterReloadMode(unsigned int timerBaseRegister, unsigned int reloadMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, timerBaseRegister);

	HWREG(timerBaseRegister + TCLR) |= reloadMode;
}

/*
 *	\brief:
 *	\param: timerBaseRegister		base address of timer
 *	\param: compareMode				0x00 compare mode disabled
 *									0x20 compare mode enabled
 */
static void setTimerCounterCompareMode(unsigned int timerBaseRegister, unsigned int compareMode)
{
	DMTimerWaitForWrite(DMTIMER_WRITE_POST_TCLR, timerBaseRegister);

	HWREG(timerBaseRegister + TCLR) |= compareMode;
}

/*
 *	\brief:
 *	\param: 	timer		number of used timer
	\return: 	base register address of timer
 */
static unsigned int getTimerBaseRegisterAddress(Timer_t timer)
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

unsigned int DMTimerWritePostedStatusGet(unsigned int baseAdd)
{
    return (HWREG(baseAdd + TWPS));
}


static void DMTimerWaitForWrite(reg, baseAdd)
{
	//if(HWREG(baseAdd + TSICR) & DMTIMER_TSICR_POSTED)\

	while((reg & DMTimerWritePostedStatusGet(baseAdd)))	// When equal to 1, a write is pending to the TCLR register
		;
}
