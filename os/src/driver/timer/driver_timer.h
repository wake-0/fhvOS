/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */
#include "../../hal/timer/hal_timer.h"
#include "../driver.h"
#include "../../hal/interrupt/hal_interrupt.h"

typedef void (*ISR)(void);

// functions for timer access

extern int TimerStart(uint16_t timer);
extern int TimerStop(uint16_t timer);
extern void TimerReset(uint16_t timer);
extern int TimerSetUp(uint16_t timer);
extern void TimerCountingHalt(Timer_t timer);
extern void TimerCountingResume(Timer_t timer);
extern void TimerValueLoad(Timer_t timer, unsigned int value);
extern void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode);
extern void TimerInterruptEnable(uint16_t timer, uint8_t timerIrq);
extern void TimerInterruptDisable(Timer_t timer, unsigned timerIrq);
extern void TimerConfigureInterrupt(Timer_t timer, ISR intHandler, unsigned int timeInMilis);
extern int TimerSetCounterValues(uint16_t timer, char * notUsed, uint16_t timerCounterValue);
extern int TimerCurrentValueGet(uint16_t timer, char * notUsed, uint16_t timerRegister);
extern int TimerConfigureCyclicInterrupt(uint16_t timer, uint16_t timeInMilis, uint8_t interruptMode, uint8_t priority, intHandler_t intHandler);

extern void TimerClockConfig(uint16_t timer, uint16_t clkSource);
