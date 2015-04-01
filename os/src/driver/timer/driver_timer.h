/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */
#include "../../hal/timer/hal_timer.h"


typedef void (*ISR)(void);

// functions for timer access

extern void TimerStart(Timer_t timer);
extern void TimerStop(Timer_t timer);
extern void TimerReset(Timer_t timer);
extern void TimerCountingHalt(Timer_t timer);
extern void TimerCountingResume(Timer_t timer);
extern void TimerValueLoad(Timer_t timer, unsigned int value);
extern void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode);
extern void TimerInterruptEnable(Timer_t timer, unsigned timerIrq);
extern void TimerInterruptDisable(Timer_t timer, unsigned timerIrq);
extern void TimerConfigureInterrupt(Timer_t timer, ISR intHandler, unsigned int timeInMilis);



