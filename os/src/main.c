
#include <stdio.h>
#include "driver/uart/driver_uart.h"
#include "driver/timer/driver_timer.h"
#include "hal/interrupt/hal_interrupt.h"

#include "driver/manager/driver_manager.h"
#include "systemapi/systemcalls.h"
#include <stdlib.h>
#include "hal/cpu/coprocessor.h"
#include "devicemanager/devicemanager.h"
#include "scheduler/scheduler.h"
#include "driver/cpu/driver_cpu.h"

extern address_t GetContext(void);
extern void CPUSwitchToPrivilegedMode(void);
extern void CPUSwitchToUserMode(void);
extern void TimerInterruptStatusClear(Timer_t timer, unsigned int interruptNumber);
boolean_t timerISR(address_t context);

void led1(void)
{
	device_t led = DeviceManagerGetDevice("LED0", 4);
	volatile int i;
	DeviceManagerOpen(led);

	while(1)
	{
		//printf("ON\n");
		DeviceManagerWrite(led, "1", 1);
		for(i = 0; i < 0x0200000; i++);
		DeviceManagerWrite(led, "0", 1);
		//printf("OFF\n");
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
uint16_t timeInMilis = 1000;
//static volatile driver_t* timerDriver;

int foo(int value);

int main(void)
{
	//DriverManagerInit();
	DeviceManagerInit();

	device_t led3 = DeviceManagerGetDevice("LED3", 4);
	DeviceManagerOpen(led3);
	DeviceManagerWrite(led3, "1", 1);

	SchedulerInit();
	SchedulerStartProcess(&led2);
	SchedulerStartProcess(&led1);

	timer2 = DeviceManagerGetDevice("TIMER2", 6);
	DeviceManagerInitDevice(timer2);

	device_t cpu = DeviceManagerGetDevice("CPU", 3);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE, 0, NULL, 0);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC, 0, NULL, 0);

	uint16_t timeInMilis = 5000; // VALUE WAS 10
	uint16_t interruptMode = 0x02; // overflow
	uint16_t priority = 0x1;

	DeviceManagerIoctl(timer2, timeInMilis, interruptMode, (char*) timerISR, priority);
	DeviceManagerOpen(timer2);

	//led1();
	while(1)
	{
		volatile int i = 0;
		volatile int j = 0;

	}

}

boolean_t timerISR(address_t context)
{
	// volatile address_t spa = GetContext();
	volatile context_t* spaContext = (context_t*) context;

	//printf("Timer ISR\n");

	DeviceManagerWrite(timer2, DISABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	DeviceManagerWrite(timer2, CLEAR_INTERRUPT_STATUS, TIMER_IRQ_OVERFLOW);
	//timerDriver->write(TIMER2, DISABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	//timerDriver->write(TIMER2, CLEAR_INTERRUPT_STATUS, TIMER_IRQ_OVERFLOW);

	flagIsr = 1;


	SchedulerRunNextProcess(spaContext);

	DeviceManagerWrite(timer2, ENABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	DeviceManagerOpen(timer2);
	//timerDriver->write(TIMER2, ENABLE_INTERRUPTS, TIMER_IRQ_OVERFLOW);
	//timerDriver->open(TIMER2);

	return FALSE;
}
