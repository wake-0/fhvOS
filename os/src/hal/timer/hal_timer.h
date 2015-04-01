/*
 * timer.h
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#define NUMBER_OF_TIMERS	8
#define DMTIMER_TCLR_START	(0x01)
#define DMTIMER_TCLR_STOP	(0xFFFFFFFE)
#define DMTIMER_WRITE_POST_TCRR DMTIMER_TWPS_W_PEND_TCLR
#define DMTIMER_WRITE_POST_TCLR	DMTIMER_TWPS_W_PEND_TCLR
#define DMTIMER_IS_RESET_DONE (DMTIMER_TIOCP_CFG_SOFTRESET_DONE)

typedef enum {TIMER0, TIMER1_MS, TIMER2,
	TIMER3, TIMER4, TIMER5, TIMER6, TIMER7
	} Timer_t;


extern void TimerHalEnableInterrupts(unsigned int baseRegister, unsigned int timerIrq);
extern void TimerHalDisableInterrupts(unsigned int baseRegister, unsigned int timerIrq);
void TimerHalClearInterruptStatus(unsigned int baseRegister, unsigned int timerIrq);
extern unsigned int TimerHalGetTimerBaseAddress(Timer_t timer);
extern unsigned int TimerHalGetMuxRegisterAddress(Timer_t timer);
extern unsigned int TimerHalGetClockControlRegisterAddress(Timer_t timer);
extern unsigned int TimerHalGetInterruptNumber(Timer_t timer);
extern void TimerHalSetClockSettings(unsigned int timerMuxSelectionRegister, unsigned int timerClockControlRegister, unsigned int clkSource);
extern void TimerHalEnableClockPrescaler(unsigned int baseRegister, unsigned int prescalerValue);
extern void TimerHalDisableClockPrescaler(unsigned int baseRegister);
extern void TimerHalClearIrqPendingFlag(unsigned int baseRegister, unsigned int timerIrq);
extern unsigned int TimerHalGetPostedStatus(unsigned int baseRegister);
extern void TimerHalWriteIrqStatusRawRegister(unsigned int baseRegister, unsigned int timerIrq);
extern void TimerHalStart(unsigned int baseRegister);
extern void TimerHalStop(unsigned int baseRegister);
extern void TimerHalSetCounterValue(unsigned int baseRegister, unsigned int counterValue);
extern unsigned int TimerHalGetCounterValue(unsigned int baseRegiser);
extern void TimerHalSetCounterReloadValue(unsigned int baseRegister, unsigned int timerCounterReloadValue);
extern void TimerHalResetCounterRegister(unsigned int baseRegister, unsigned int resetValue);
extern void TimerHalConfigureMode(unsigned int baseRegister, unsigned int compareMode, unsigned int reloadMode);
extern void TimerHalSetTriggerMode(unsigned int baseRegister, unsigned int triggerMode);
extern void TimerHalSetCaptureMode(unsigned int baseRegister, unsigned int captureMode);
extern void TimerHalSetOutputPinMode(unsigned int baseRegister, unsigned int controlRegister, unsigned int pinMode);
extern void TimerHalSetTransitionCaptureMode(unsigned int baseRegister, unsigned int transitionMode);
