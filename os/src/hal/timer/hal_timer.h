/*
 * timer.h
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#include "../../platform/platform.h"

#define NUMBER_OF_TIMERS	8
#define DMTIMER_TCLR_START	(0x01)
#define DMTIMER_TCLR_STOP	(0xFFFFFFFE)
#define DMTIMER_WRITE_POST_TCRR DMTIMER_TWPS_W_PEND_TCLR
#define DMTIMER_WRITE_POST_TCLR	DMTIMER_TWPS_W_PEND_TCLR
#define DMTIMER_IS_RESET_DONE (DMTIMER_TIOCP_CFG_SOFTRESET_DONE)

typedef enum {TIMER0, TIMER1_MS, TIMER2,
	TIMER3, TIMER4, TIMER5, TIMER6, TIMER7
	} Timer_t;


extern void TimerHalEnableInterrupts(address_t baseAddress, unsigned int timerIrq);
extern void TimerHalDisableInterrupts(address_t baseAddress, unsigned int timerIrq);
void TimerHalClearInterruptStatus(address_t baseAddress, unsigned int timerIrq);
extern unsigned int TimerHalGetTimerBaseAddress(address_t baseAddress);
extern unsigned int TimerHalGetMuxRegisterAddress(address_t baseAddress);
extern unsigned int TimerHalGetClockControlRegisterAddress(address_t baseAddress);
extern unsigned int TimerHalGetInterruptNumber(unsigned int timer);
extern void TimerHalSetClockSettings(unsigned int timer, unsigned int timerMuxSelectionRegister, unsigned int timerClockControlRegister, unsigned int clkSource);
extern void TimerHalEnableClockPrescaler(address_t baseAddress, unsigned int prescalerValue);
extern void TimerHalDisableClockPrescaler(address_t baseAddress);
extern void TimerHalClearIrqPendingFlag(address_t baseAddress, unsigned int timerIrq);
extern unsigned int TimerHalGetPostedStatus(address_t baseAddress);
extern void TimerHalWriteIrqStatusRawRegister(address_t baseAddress, unsigned int timerIrq);
extern void TimerHalStart(address_t baseAddress);
extern void TimerHalStop(address_t baseAddress);
extern void TimerHalSetCounterValue(address_t baseAddress, unsigned int counterValue);
extern unsigned int TimerHalGetCounterValue(address_t baseAddress);
extern void TimerHalSetCounterReloadValue(address_t baseAddress, unsigned int timerCounterReloadValue);
extern void TimerHalResetCounterRegister(address_t baseAddress, unsigned int resetValue);
extern void TimerHalConfigureMode(address_t baseAddress, unsigned int compareMode, unsigned int reloadMode);
extern void TimerHalSetTriggerMode(address_t baseAddress, unsigned int triggerMode);
extern void TimerHalSetCaptureMode(address_t baseAddress, unsigned int captureMode);
extern void TimerHalSetOutputPinMode(address_t baseAddress, unsigned int controlRegister, unsigned int pinMode);
extern void TimerHalSetTransitionCaptureMode(address_t baseAddress, unsigned int transitionMode);
unsigned int TimerHalGetReloadValue(address_t baseAddress);
