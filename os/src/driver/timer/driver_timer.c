/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "driver_timer.h"
#include "../../hal/interrupt/hal_interrupt.h"

#define TIMER_RESET_VALUE 0x00



/*
 *	\brief:		Driver-Function for starting the counting of the specified timer
 *	\param: 	timer				specified timer
 */
void TimerStart(Timer_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalStart(timerBaseRegister);
}

/*
 *	\brief:		Driver-Function for stopping the counting of the specified timer
 *	\param: 	timer				specified timer
 */
void TimerStop(Timer_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalStop(timerBaseRegister);
}

/*
 *	\brief:		Driver-Function for resetting the specified timer.
 *				Timer will be stopped, timer value will be set to 0.
 *	\param: 	timer				specified timer
 */
void TimerReset(Timer_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalResetCounterRegister(timerBaseRegister, TIMER_RESET_VALUE);
}

/*
 *	\brief:		Driver-Function for setting the counter value of the specified timer.
 *	\param: 	timer				specified timers
*	\param: 	timerCounterValue	new counter value
 */
void TimerCounterValueSet(Timer_t timer, unsigned int timerCounterValue)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalSetCounterValue(timerBaseRegister, timerCounterValue);
}

void TimerReloadValueSet(Timer_t timer, unsigned int timerCounterReloadValue)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalSetCounterReloadValue(timerBaseRegister, timerCounterReloadValue);
}


void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalConfigureMode(timerBaseRegister, compareMode, reloadMode);
}


void TimerConfigureCyclicInterrupt(Timer_t timer, intHandler_t intHandler, unsigned int timeInMilis)
{
	unsigned int timerBaseRegister 	= TimerHalGetTimerBaseAddress(timer);
	unsigned int interruptNumber 	= TimerHalGetInterruptNumber(timer);

	// calculate value to load to tldr

	// register ISR to timer
	InterruptHandlerRegister(interruptNumber, intHandler);
	InterruptPrioritySet(interruptNumber, 0);
	InterruptHandlerEnable(interruptNumber);
}

void TimerSetUp(Timer_t timer, unsigned int initialValue, unsigned int reloadValue, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerCounterValueSet(timer, initialValue);
	TimerReloadValueSet(timer, reloadValue);
	TimerHalConfigureMode(timerBaseRegister, compareMode, reloadMode);
}



void TimerInterruptEnable(Timer_t timer, unsigned timerIrq)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalEnableInterrupts(timerBaseRegister, timerIrq);
}

void TimerInterruptDisable(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalDisableInterrupts(timerBaseRegister, timerIrq);
}

void TimerInterruptStatusClear(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalClearInterruptStatus(timerBaseRegister, timerIrq);
}

unsigned int TimerCurrentValueGet(Timer_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	return TimerHalGetCounterValue(timerBaseRegister);
}

void TimerTriggerSoftwareInterrupt(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalWriteIrqStatusRawRegister(timerBaseRegister, timerIrq);
}

void TimerIrqPendingClear(Timer_t timer, unsigned int timerIrq)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalClearIrqPendingFlag(timerBaseRegister, timerIrq);
}

void TimerConfigureCapture(Timer_t timer, unsigned int compareMode, unsigned int compareValue)
{

}


void TimerClockConfig(Timer_t timer, unsigned int clkSource)
{
	unsigned int timerBaseRegister 			= TimerHalGetTimerBaseAddress(timer);
	unsigned int timerMuxSelectionRegister 	= TimerHalGetMuxRegisterAddress(timer);
	unsigned int timerClockControlRegister 	= TimerHalGetClockControlRegisterAddress(timer);

	TimerHalSetClockSettings(timerMuxSelectionRegister, timerClockControlRegister, clkSource);
}


void TimerCountingHalt(Timer_t timer)
{

}

void TimerCountingResume(Timer_t timer)
{

}
