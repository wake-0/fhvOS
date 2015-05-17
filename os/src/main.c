
#include <stdio.h>
#include "driver/uart/driver_uart.h"
#include "driver/timer/driver_timer.h"
#include "hal/interrupt/hal_interrupt.h"

#include "driver/manager/driver_manager.h"
#include "systemapi/systemcalls.h"
#include <stdlib.h>
#include "devicemanager/devicemanager.h"
#include "scheduler/scheduler.h"
#include "driver/cpu/driver_cpu.h"
#include "console/console.h"
#include "processmanager/processmanager.h"

extern address_t GetContext(void);
extern void CPUSwitchToPrivilegedMode(void);
extern void CPUSwitchToUserMode(void);
extern void TimerInterruptStatusClear(Timer_t timer, unsigned int interruptNumber);

void simpleProc1(int argc, char** argv)
{
	while(1)
	{
		volatile int i;
		printf("Process 1\n");
		for(i = 0; i < 0x0200000; i++);

	}
}
void simpleProc2(int argc, char** argv)
{
	while(1)
	{
		volatile int i;
		printf("---------Process 2\n");
		for(i = 0; i < 0x0200000; i++);
	}
}

void led1(int argc, char** argv)
{
	device_t led = DeviceManagerGetDevice("LED0", 4);
	//device_t uart = DeviceManagerGetDevice("UART0", 5);

	//DeviceManagerOpen(uart);

	volatile int i;
	DeviceManagerOpen(led);

	while(1)
	{
		//printf("ON\n");
		char buf[12];
		for(i = 0; i < 10; i++) {
			//printf("led1: i=%i\n", i);
			sprintf(buf, "led1: i=%d\r\n", i);
			//DeviceManagerWrite(uart, buf, 12);
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

void led2(int argc, char** argv)
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

void process3(int argc, char** argv)
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

	device_t uart = DeviceManagerGetDevice("UART0", 5);
	ConsoleInit(uart);
	ProcessManagerInit();


	ProcessManagerStartProcess("sp1", &simpleProc1);
	ProcessManagerStartProcess("sp2", &simpleProc2);

	ProcessManagerStartProcess("led1", &led1);
	ProcessManagerStartProcess("led2", &led2);
	ProcessManagerStartProcess("proc3", &process3);
	ProcessManagerStartProcess("tty", &ConsoleProcess);


	// Set up the timer
	//DeviceManagerInitDevice(timer);

	while(1)
	{
		volatile int i = 0;
		volatile int j = 0;

	}

}
