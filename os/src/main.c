
//#include <stdio.h>

#include "driver/uart/driver_uart.h"
#include "driver/timer/driver_timer.h"
#include "hal/interrupt/hal_interrupt.h"
#include "hal/am335x/hw_cm_dpll.h"
#include "hal/am335x/hw_cm_per.h"
#include "hal/am335x/soc_AM335x.h"
#include "hal/am335x/hw_timer.h"
#include "hal/am335x/hw_types.h"
#include "hal/am335x/hw_cm_dpll.h"
#include "driver/manager/driver_manager.h"
#include <time.h>


extern void CPUSwitchToPrivilegedMode(void);
extern void CPUSwitchToUserMode(void);
extern void TimerInterruptStatusClear(Timer_t timer, unsigned int interruptNumber);
void timerISR(void);


static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;

driver_t* timerDriver;
time_t seconds;
uint16_t timeInMilis = 2000;

int main(void)
{
	DriverManagerInit();
	timerDriver = DriverManagerGetDriver(DRIVER_ID_TIMER);
	timerDriver->init(TIMER2);

	CPUSwitchToUserMode();
	CPUSwitchToPrivilegedMode();

	// ----- INTERRUPT settings -----
	InterruptMasterIRQEnable();
	InterruptResetAINTC();

	// ----- TIMER settings -----
	uint16_t timeInMilis = 5000;
	uint16_t interruptMode = 0x02; // overflow
	uint16_t priority = 0x1;
	timerDriver->ioctl(TIMER2, timeInMilis, interruptMode, (char*) timerISR, priority);

	// ----- TIMER start -----
	timerDriver->open(TIMER2);

	volatile int counter = 0;
	while(cntValue)
	{
		counter++;
		if(flagIsr == 1)
		{
			counter = 0;
			//printf("\nOverflow\n");
			cntValue--;
			flagIsr = 0;
		}
	}

	while(1);
}


void timerISR(void)
{
	timerDriver->write(TIMER2, DISABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	timerDriver->write(TIMER2, CLEAR_INTERRUPT_STATUS, TIMER_IRQ_OVERFLOW);

	flagIsr = 1;

	timerDriver->write(TIMER2, ENABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	timerDriver->open(TIMER2);
}
