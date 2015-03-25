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
#define DMTIMER_TCLR_ENABLE_COMPARE 0x20
#define DMTIMER_TCLR_DISABLE_COMPARE 0x00
#define DMTIMER_TCLR_NO_TRIGGER 0x0
#define DMTIMER_TCLR_OVERFLOW_TRIGGER 0x400
#define DMTIMER_TCLR_OVERFLOW_MATCH_TRIGGER 0x800
#define DMTIMER_TCLR_SINGLE_CAPTURE 0x0
#define DMTIMER_TCLR_CAPTURE_ON_SECOND_EVENT 0x4000
#define DMTIMER_TCLR_ONE_SHOT_TIMER 0x0
#define DMTIMER_TCLR_AUTO_RELOAD 0x02
#define DMTIMER_TCLR_NO_CAPTURE 0x0
#define DMTIMER_TCLR_CAPTURE_ON_TRANSITION_LOW_TO_HIGH 0x100
#define DMTIMER_TCLR_CAPTURE_ON_TRANSITION_HIGH_TO_LOW 0x200
#define DMTIMER_TCLR_CAPTURE_ON_BOTH_EDGE_TRANSITION 0x300
#define DMTIMER_TCLR_PINMODE_PULSE 0x0
#define DMTIMER_TCLR_PINMODE_TOGGLE 0x1000

typedef enum {enableCompare, disableCompare} timerCompareMode_t;
typedef enum {noTrigger, overflowTrigger, overflowAndMatchTrigger} timerTriggerMode_t;
typedef enum {singleCapture, onSecondEvent} timerCaptureMode_t;
typedef enum {oneShotTimer, autoReload} timerReloadMode_t;
typedef enum {noCapture, lowToHigh, highToLow, bothEdges} timerTransitionMode_t;
typedef enum {pulse, toggle} timerPinMode_t;
typedef enum {captureInterrupt, overflowInterrupt, matchInterrupt} timerInterrupts_t;

typedef enum {TIMER0, TIMER1_MS, TIMER2,
	TIMER3, TIMER4, TIMER5, TIMER6, TIMER7
	} Timer_t;

