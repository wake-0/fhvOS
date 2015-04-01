/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "driver_timer.h"
#include "../../hal/interrupt/interrupt.h"

#define TIMER_RESET_VALUE 0x00


static int timers[NUMBER_OF_TIMERS];



/*
 *	\brief:		Driver-Function for starting the counting of the specified timer
 *	\param: 	timer				specified timer
 */
void TimerStart(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	//resetTimerCounterRegister(timerBaseRegister, TIMER_RESET_VALUE);

	startTimer(timerBaseRegister);
}

/*
 *	\brief:		Driver-Function for stopping the counting of the specified timer
 *	\param: 	timer				specified timer
 */
void TimerStop(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	stopTimer(timerBaseRegister);
}

/*
 *	\brief:		Driver-Function for resetting the specified timer.
 *				Timer will be stopped, timer value will be set to 0.
 *	\param: 	timer				specified timer
 */
void TimerReset(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	resetTimerCounterRegister(timerBaseRegister, TIMER_RESET_VALUE);
}

/*
 *	\brief:		Driver-Function for setting the counter value of the specified timer.
 *	\param: 	timer				specified timers
*	\param: 	timerCounterValue	new counter value
 */
void TimerCounterValueSet(Timer_t timer, unsigned int timerCounterValue)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	setTimerCounterValue(timerBaseRegister, timerCounterValue);
}

void TimerReloadValueSet(Timer_t timer, unsigned int timerCounterReloadValue)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	setTimerCounterReloadValue(timerBaseRegister, timerCounterReloadValue);
}


void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	configureTimerMode(timerBaseRegister, compareMode, reloadMode);
}



void TimerConfigureCyclicInterrupt(Timer_t timer, intHandler_t intHandler, unsigned int timeInMilis)
{
	unsigned int timerBaseRegister 	= getTimerBaseRegisterAddress(timer);
	unsigned int interruptNumber 	= getTimerInterruptNumber(timer);

	// calculate value to load to tldr

	// register ISR to timer
	InterruptHandlerRegister(interruptNumber, intHandler);
	InterruptPrioritySet(interruptNumber, 0);
	InterruptHandlerEnable(interruptNumber);
}

void TimerSetUp(Timer_t timer, unsigned int initialValue, unsigned int reloadValue, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	TimerCounterValueSet(timer, initialValue);
	TimerReloadValueSet(timer, reloadValue);
	configureTimerMode(timerBaseRegister, compareMode, reloadMode);
}



void TimerInterruptEnable(Timer_t timer, unsigned timerIrq)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	enableTimerInterrupts(timerBaseRegister, timerIrq);
}

void TimerInterruptDisable(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	disableTimerInterrupts(timerBaseRegister, timerIrq);
}

void TimerInterruptStatusClear(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	clearInterruptStatus(timerBaseRegister, timerIrq);
}

unsigned int TimerCurrentValueGet(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	return getTimerCounterValue(timerBaseRegister);
}

void TimerTriggerSoftwareInterrupt(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	writeIrqStatusRawRegister(timerBaseRegister, timerIrq);
}

void TimerIrqPendingClear(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	clearTimerIrqPendingFlag(timerBaseRegister, timerIrq);
}

void TimerConfigureCapture(Timer_t timer, unsigned int compareMode, unsigned int compareValue)
{

}


void TimerClockConfig(Timer_t timer, unsigned int clkSource)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	unsigned int timerMuxSelectionRegister = getTimerMuxSelectionRegisterAddress(timer);
	unsigned int timerClockControlRegister = getTimerClockControlRegisterAddress(timer);

	selectClockSourceForTimer(timerMuxSelectionRegister, timerClockControlRegister, clkSource);
}



void TimerCountingHalt(Timer_t timer)
{

}

void TimerCountingResume(Timer_t timer)
{

}
