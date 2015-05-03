
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

void led1(void)
{
	device_t led = DeviceManagerGetDevice("LED0", 4);
	device_t uart = DeviceManagerGetDevice("UART0", 5);

	DeviceManagerOpen(uart);

	volatile int i;
	DeviceManagerOpen(led);

	while(1)
	{
		//printf("ON\n");
		char buf[12];
		for(i = 0; i < 10; i++) {
			//printf("led1: i=%i\n", i);
			sprintf(buf, "led1: i=%d\r\n", i);
			DeviceManagerWrite(uart, buf, 12);
		}

		DeviceManagerWrite(led, "1", 1);
		for(i = 0; i < 0x0200000; i++);
		//printf("%i\n", i);
		DeviceManagerWrite(led, "0", 1);
		for(i = 0; i < 10; i++) {
			//printf("led1: i=%i\n", i);
		}

		//printf("OFF\n");
		for(i = 0; i < 0x0200000; i++);

		for(i = 0; i < 10; i++) {
			//printf("led1: i=%i\n", i);
		}
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

		for(i = 0; i < 10; i++) {
			printf("led2: i=%i\n", i);
		}

		for(i = 0; i < 0x0200000; i++);
		DeviceManagerWrite(led, "0", 1);


		for(i = 0; i < 10; i++) {
			printf("led2: i=%i\n", i);
		}

		for(i = 0; i < 0x0200000; i++);

		for(i = 0; i < 10; i++) {
			printf("led2: i=%i\n", i);
		}
	}
}

void process3(void)
{
	volatile int i = 0;
	for(i = 0; i < 10; i++) {
		printf("process3: i=%i\n", i);
	}

	for(i = 0; i < 0x0200000; i++);

	printf("Process 3 is ending now...\n");
}

static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;

device_t timer2;
uint16_t timeInMilis = 1000;
//static volatile driver_t* timerDriver;

int foo(int value);

int main(void)
{
	DeviceManagerInit();

	device_t led3 = DeviceManagerGetDevice("LED3", 4);
	DeviceManagerOpen(led3);
	DeviceManagerWrite(led3, "1", 1);

	SchedulerInit();
	SchedulerStartProcess(&led2);
	SchedulerStartProcess(&led1);
	SchedulerStartProcess(&process3);


	device_t timer = DeviceManagerGetDevice("TIMER2", 6);

	// Set up the timer
	DeviceManagerInitDevice(timer);

	device_t cpu = DeviceManagerGetDevice("CPU", 3);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_MASTER_IRQ_ENABLE, 0, NULL, 0);
	DeviceManagerIoctl(cpu, DRIVER_CPU_COMMAND_INTERRUPT_RESET_AINTC, 0, NULL, 0);

	//SchedulerStart(timer);
	//led1();
	while(1)
	{
		volatile int i = 0;
		volatile int j = 0;

	}

}
