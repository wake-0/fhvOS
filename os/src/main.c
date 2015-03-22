
//#include <stdio.h>

#include "driver/uart/uartDriver.h"
#include "driver/timer/driver_timer.h"

void CPUirqd1(void);
void CPUirqe1(void);
void CPUSwitchToUserMode1(void);
void CPUSwitchToPrivilegedMode1(void);
interrupt void undef_handler(void);
interrupt void fiq_handler(void);
interrupt void irq_handler(void);

int main(void)
{
	TimerReset(TIMER0);
	TimerValueLoad(TIMER0, 0x10);

	TimerStart(TIMER0);

	CPUirqe1();

	CPUSwitchToUserMode1();

	CPUSwitchToPrivilegedMode1();

	while(1)
	{
	}
}

#pragma INTERRUPT(udef_handler, UDEF)
interrupt void udef_handler() {
	printf("udef interrupt\n");
}

#pragma INTERRUPT(fiq_handler, FIQ)
interrupt void fiq_handler() {
	printf("fiq interrupt\n");
}

/**
 * Is called on any prefetch abort.
 */
#pragma INTERRUPT(pabt_handler, PABT)
interrupt void pabt_handler() {
	printf("pabt interrupt\n");
}




void CPUSwitchToPrivilegedMode1(void)
{
    asm("    SWI   #458752");
}

void CPUSwitchToUserMode1(void)
{
    asm("    mrs     r0, CPSR\n\t"
        "    bic     r0, r0, #0x0F\n\t"
        "    orr     r0, r0, #0x10\n\t "
        "    msr     CPSR_c, r0");
}

void CPUirqd1(void)
{

    // Disable IRQ in CPSR
    asm("    mrs     r0, CPSR\n\t"
        "    orr     r0, r0, #0x80\n\t"
        "    msr     CPSR_c, r0");
}

void CPUirqe1(void)
{
    // Enable IRQ in CPSR
    asm("    mrs     r0, CPSR\n\t"
        "    bic     r0, r0, #0x80\n\t"
        "    msr     CPSR_c, r0");
}
