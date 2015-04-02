/*
 * driver_timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "driver_timer.h"

#define TIMER_RESET_VALUE 							0x00
#define TIMER_DEFAULT_VALUE 						0xFF000000
#define TIMER_COMPARE_MODE_DISABLED 				0x00
#define TIMER_AUTO_RELOAD_ENABLED 					0x02
#define TIMER_CLK_SELECT_M_OSC 						0x01
#define TICKS_PER_MILISECOND						25000
#define TIMER_OVERFLOW_VALUE						0xFFFFFFFF

/*
 *	\brief:		Driver-Function for starting the counting of the specified timer
 *	\param: 	timer				specified timer
 */
int TimerStart(uint16_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalStart(timerBaseRegister);
	return DRIVER_OK;
}

/*
 *	\brief:		Driver-Function for stopping the counting of the specified timer
 *	\param: 	timer				specified timer
 */
int TimerStop(uint16_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalStop(timerBaseRegister);
	return DRIVER_OK;
}

/*
 *	\brief:		Driver-Function for resetting the specified timer.
 *				Timer will be stopped, timer value will be set to 0.
 *	\param: 	timer				specified timer
 */
void TimerReset(uint16_t timer)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalResetCounterRegister(timerBaseRegister, TIMER_RESET_VALUE);
}

/*
 *	\brief:		Driver-Function for setting the counter value of the specified timer.
 *	\param: 	timer				specified timers
*	\param: 	timerCounterValue	new counter value
 */
void TimerCounterValueSet(uint16_t timer, unsigned int timerCounterValue)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalSetCounterValue(timerBaseRegister, timerCounterValue);
}

void TimerReloadValueSet(uint16_t timer, unsigned int timerCounterReloadValue)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalSetCounterReloadValue(timerBaseRegister, timerCounterReloadValue);
}

//int (*write)(uint16_t pin, char* buf, uint16_t length);

int TimerSetCounterValues(uint16_t timer, char * notUsed, uint16_t timerCounterValue)
{
	TimerCounterValueSet(timer, timerCounterValue);
	TimerReloadValueSet(timer, timerCounterValue);
	return DRIVER_OK;
}

void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalConfigureMode(timerBaseRegister, compareMode, reloadMode);
}

//(uint16_t pin, uint16_t cmd, h)
int TimerConfigureCyclicInterrupt(uint16_t timer, uint16_t timeInMilis, uint8_t interruptMode, uint8_t priority, intHandler_t intHandler)
{
	unsigned int timerBaseRegister 	= TimerHalGetTimerBaseAddress(timer);
	unsigned int interruptNumber 	= TimerHalGetInterruptNumber(timer);

	// calculate value to load to tldr
	unsigned int timerCounterValue = 0;

	if(0 == timeInMilis)
	{
		return DRIVER_ERROR;
	}
	else
	{
		timerCounterValue = TIMER_OVERFLOW_VALUE - (timeInMilis * TICKS_PER_MILISECOND);
	}

	TimerCounterValueSet(timer, timerCounterValue);
	TimerReloadValueSet(timer, timerCounterValue);

	// register ISR to timer
	InterruptHandlerRegister(interruptNumber, intHandler);
	InterruptPrioritySet(interruptNumber, 0);
	InterruptHandlerEnable(interruptNumber);

	// enable timer interrupts
	TimerInterruptEnable(timer, interruptMode);

	return DRIVER_OK;
}

int TimerSetUp(uint16_t timer)
{
	TimerReset(timer);
	TimerClockConfig(timer, TIMER_CLK_SELECT_M_OSC);
	TimerCounterValueSet(timer, TIMER_DEFAULT_VALUE);
	TimerReloadValueSet(timer, TIMER_DEFAULT_VALUE);
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);
	TimerHalConfigureMode(timerBaseRegister, TIMER_COMPARE_MODE_DISABLED, TIMER_AUTO_RELOAD_ENABLED);
	return DRIVER_OK;
}



void TimerInterruptEnable(uint16_t timer, uint8_t timerIrq)
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

int TimerCurrentValueGet(uint16_t timer, char * notUsed, uint16_t timerRegister)
{
	unsigned int timerBaseRegister = TimerHalGetTimerBaseAddress(timer);

	switch(timerRegister)
	{
		case 0: default:
			return TimerHalGetCounterValue(timerBaseRegister);
		case 1:
			return TimerHalGetReloadValue(timerBaseRegister);
	}
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


void TimerClockConfig(uint16_t timer, uint16_t clkSource)
{
	unsigned int timerBaseRegister 			= TimerHalGetTimerBaseAddress(timer);
	unsigned int timerMuxSelectionRegister 	= TimerHalGetMuxRegisterAddress(timer);
	unsigned int timerClockControlRegister 	= TimerHalGetClockControlRegisterAddress(timerBaseRegister);

	TimerHalSetClockSettings(timerMuxSelectionRegister, timerClockControlRegister, clkSource);
}


void TimerCountingHalt(Timer_t timer)
{

}

void TimerCountingResume(Timer_t timer)
{

}
