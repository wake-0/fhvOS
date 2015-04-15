
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
#include "systemapi/systemcalls.h"
#include <stdlib.h>
#include "hal/cpu/coprocessor.h"
#include "devicemanager/devicemanager.h"
#include "scheduler/scheduler.h"
#include "driver/cpu/driver_cpu.h"

extern void CPUSwitchToPrivilegedMode(void);
extern void CPUSwitchToUserMode(void);
extern void TimerInterruptStatusClear(Timer_t timer, unsigned int interruptNumber);
void timerISR(void);

void led1(void)
{
	device_t led = DeviceManagerGetDevice("LED0", 4);
	volatile int i;
	DeviceManagerOpen(led);

	while(1)
	{
		DeviceManagerWrite(led, "1", 1);
		for(i = 0; i < 0x0200000; i++);
		DeviceManagerWrite(led, "0", 1);
		for(i = 0; i < 0x0200000; i++);
	}
}

void led2(void)
{
	device_t led = DeviceManagerGetDevice("LED1", 4);
	volatile int i;
	DeviceManagerOpen(led);

	while(1)
	{
		DeviceManagerWrite(led, "1", 1);
		for(i = 0; i < 0x0200000; i++);
		DeviceManagerWrite(led, "0", 1);
		for(i = 0; i < 0x0200000; i++);
	}
}

static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;

device_t timer2;
uint16_t timeInMilis = 2000;


int main(void)
{
	DriverManagerInit();

	SchedulerInit();
	SchedulerStartProcess(&led1);
	SchedulerStartProcess(&led2);


	systemCallMessage_t * message = (systemCallMessage_t * )malloc(sizeof(systemCallMessage_t));
	message->systemCallNumber = SYS_CHMOD;
	message->messageArgs.arg1 = 0x10;		// USER MODE
	SystemCall(message);

	message->messageArgs.arg1 = 0x1F;		// SYSTEM MODE
	SystemCall(message);

	device_t cpu = DeviceManagerGetDevice("CPU", 3);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE, 0, NULL, 0);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC, 0, NULL, 0);

	timer2 = DeviceManagerGetDevice("TIMER2", 6);
	DeviceManagerInitDevice(timer2);
	uint16_t timeInMilis = 5000;
	uint16_t interruptMode = 0x02; // overflow
	uint16_t priority = 0x1;

	DeviceManagerIoctl(timer2, timeInMilis, interruptMode, (char*) timerISR, priority);
	DeviceManagerOpen(timer2);

	while(1);

	/*
	DriverManagerInit();
	timerDriver = DriverManagerGetDriver(DRIVER_ID_TIMER);
	timerDriver->init(TIMER2);

	systemCallMessage_t * message = (systemCallMessage_t * )malloc(sizeof(systemCallMessage_t));
	message->systemCallNumber = SYS_CHMOD;
	message->messageArgs.arg1 = 0x10;		// USER MODE
	SystemCall(message);

	message->messageArgs.arg1 = 0x1F;		// SYSTEM MODE
	SystemCall(message);


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
	*/
}


void timerISR(void)
{
	DeviceManagerWrite(timer2, DISABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	DeviceManagerWrite(timer2, CLEAR_INTERRUPT_STATUS, TIMER_IRQ_OVERFLOW);
	//timerDriver->write(TIMER2, DISABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	//timerDriver->write(TIMER2, CLEAR_INTERRUPT_STATUS, TIMER_IRQ_OVERFLOW);

	flagIsr = 1;

	//SchedulerRunNextProcess();

	DeviceManagerWrite(timer2, ENABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	DeviceManagerOpen(timer2);
	//timerDriver->write(TIMER2, ENABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	//timerDriver->open(TIMER2);
}
