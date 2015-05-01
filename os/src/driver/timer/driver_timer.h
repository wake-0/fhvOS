/*
 * timer.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */
#include "../../hal/timer/hal_timer.h"
#include "../driver.h"
#include "../../hal/interrupt/hal_interrupt.h"


#define ENABLE_INTERRUPTS "ENABLE_INTERRUPTS"
#define DISABLE_INTERRUPTS "DISABLE_INTERRUPTS"
#define WRITE_COUNTER_REGISTER "WRITE_COUNTER_REGISTER"
#define WRITE_RELOAD_REGISTER "WRITE_RELOAD_REGISTER"
#define CLEAR_INTERRUPT_STATUS "CLEAR_INTERRUPT_STATUS"
#define RESET_TIMER "RESET_TIMER"
#define TIMER_IRQ_CAPTURE			(0x4)
#define TIMER_IRQ_OVERFLOW			(0x2)
#define TIMER_IRQ_MATCH			(0x1)

typedef void (*ISR)(void);

// functions for timer access

extern int TimerDriverOpen(uint16_t timer);
extern int TimerDriverClose(uint16_t timer);
extern void TimerReset(uint16_t timer);
extern int TimerDriverInit(uint16_t timer);
extern void TimerCountingHalt(Timer_t timer);
extern void TimerCountingResume(Timer_t timer);
extern void TimerValueLoad(Timer_t timer, unsigned int value);
extern void TimerModeConfigure(Timer_t timer, unsigned int compareMode, unsigned int reloadMode);
extern void TimerInterruptEnable(uint16_t timer, uint8_t timerIrq);
extern void TimerInterruptDisable(uint16_t timer, uint8_t timerIrq);
extern void TimerConfigureInterrupt(Timer_t timer, ISR intHandler, unsigned int timeInMilis);
extern int TimerDriverWrite(uint16_t timer, char * notUsed, uint16_t timerCounterValue);
extern int TimerDriverRead(uint16_t timer, char * notUsed, uint16_t timerRegister);
extern int TimerDriverIoctl(uint16_t timer, uint16_t timeInMilis, uint8_t interruptMode, char* buf, uint16_t priority);

extern void TimerClockConfig(uint16_t timer, uint16_t clkSource);
