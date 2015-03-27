/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "driver_timer.h"

#define TIMER_RESET_VALUE 0x00


static int timers[NUMBER_OF_TIMERS];



/*
 *	\brief:		Driver-Function for starting the counting of the specified timer
 *	\param: 	timer				specified timer
 */
void TimerStart(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	resetTimerCounterRegister(timerBaseRegister, TIMER_RESET_VALUE);

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
void TimerValueLoad(Timer_t timer, unsigned int timerCounterValue)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);

	setTimerCounterValue(timerBaseRegister, timerCounterValue);
}


void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
	configureTimerMode(timerBaseRegister, compareMode, reloadMode);
}



void TimerConfigureCyclicInterrupt(Timer_t timer, ISR intHandler, unsigned int timeInMilis)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
}



void TimerInterruptEnable(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
}

void TimerInterruptDisable(Timer_t timer)
{
	unsigned int timerBaseRegister = getTimerBaseRegisterAddress(timer);
}

void TimerConfigureCapture(Timer_t timer, unsigned int compareMode, unsigned int compareValue)
{

}




void TimerCountingHalt(Timer_t timer)
{

}

void TimerCountingResume(Timer_t timer)
{

}
