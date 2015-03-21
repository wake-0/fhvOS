/*
 * timer.h
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */

#define DMTIMER_TCLR_START	(0x01)
#define DMTIMER_TCLR_STOP	(0xFFFFFFFE)

#define DMTIMER_WRITE_POST_TCRR DMTIMER_TWPS_W_PEND_TCLR
#define DMTIMER_WRITE_POST_TCLR	DMTIMER_TWPS_W_PEND_TCLR

typedef enum {TIMER0, TIMER1_MS, TIMER2,
	TIMER3, TIMER4, TIMER5, TIMER6, TIMER7
	} Timer_t;


// functions for timer access

extern void TimerEnable(Timer_t timer);
extern void TimerDisable(Timer_t timer);
extern void TimerReset(Timer_t timer);
extern void TimerStart(Timer_t timer);
extern void TimerStop(Timer_t timer);
extern void TimerCountingHalt(Timer_t timer);
extern void TimerCountingResume(Timer_t timer);
extern void TimerValueLoad(Timer_t timer, unsigned int value);
extern void TimerModeConfigure(Timer_t timer, unsigned int mode);

