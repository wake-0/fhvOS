
#include <stdio.h>
#include "driver/timer/driver_timer.h"
#include "driver/uart/driver_uart.h"
#include "hal/interrupt/interrupt.h"
#include "hal/cpu/cpu.h"

static void DMTimerIsr(void);

int main(void)
{
	/*
	AintcInit();

	InterruptHandlerRegister(SYS_INT_TINT2,DMTimerIsr);

	TimerReset(TIMER0);


	CPUirqe();

	CPUSwitchToUserMode();


	TimerValueLoad(TIMER0, 0x10);

	TimerStart(TIMER0);
	while(1)
	{
	}
	*/
	UARTDriverInit(0);

	char text[12];
	int length = 12;

	while(1)
	{
		UARTDriverRead(0, text, length);

		UARTDriverWrite(0, text, length);
	}
}


static void DMTimerIsr(void)
{

    /* Disable the DMTimer interrupts */
    //DMTimerIntDisable(DMTIMER_INSTANCE, DMTIMER_INT_OVF_EN_FLAG);

    /* Clear the status of the interrupt flags */
    //DMTimerIntStatusClear(DMTIMER_INSTANCE, DMTIMER_INT_OVF_IT_FLAG);

	printf("\nTimer Interrupt occured!\n");
    //flagIsr = 1;

    /* Enable the DMTimer interrupts */
    //DMTimerIntEnable(DMTIMER_INSTANCE, DMTIMER_INT_OVF_EN_FLAG);
}
