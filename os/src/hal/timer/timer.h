/*
 * timer.h
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */


typedef enum {TIMER0, TIMER1_MS, TIMER2,
	TIMER3, TIMER4, TIMER5, TIMER6, TIMER7
	} Timer;


// functions for timer access

extern void enableTimer(Timer timer);
extern void disableTimer(Timer timer);
extern void resetTimer(Timer timer);
extern void stopTimer(Timer timer);
extern void haltTimerCounting(Timer timer);
extern void resumeTimerCounting(Timer timer);
extern void loadTimerValue(Timer timer, unsigned int value);

