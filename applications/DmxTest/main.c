#include <stdio.h>
#include <devices.h>
#include <systemcall.h>
#include <system.h>

#define DMX_MAX		6

int main(int argc, char** argv) {

	if (argc != 0)
	{
		printf("I do not accept any arguments (intentionally). You really should think about why :)\n");
		return 0;
	}
	int handle = open_device("DMX");

	char DMXBuffer[DMX_MAX];
	// Set the default dmx buffer
	int i = 0;
	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	// set tilt
	DMXBuffer[1] = 150;
	// open
	DMXBuffer[3] = 5;
	// set gradient
	DMXBuffer[2] = 125;

	volatile int pan = 0;
	volatile int direction = 0;

	while (direction != 2) {
		// sample stuff
		if (direction == 0) {
			DMXBuffer[pan] += 5;
			if (DMXBuffer[pan] == 255) {
				direction = 1;
			}
		} else if (direction == 1) {
			DMXBuffer[pan] -= 5;
			if (DMXBuffer[pan] == 0) {
				direction = 2;
			}
		}

		for (i = 0; i < DMX_MAX; i++) {
			ioctl_device(handle,0,0, &DMXBuffer[0], sizeof(DMXBuffer));
			//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
			sleep(30);
		}
		if (DMXBuffer[pan] == 0 || DMXBuffer[pan] == 255) {
			//printf("Sent DMX Buffer\n");
		}
	}

	sleep(100);

	for (i = 0; i < DMX_MAX; i++) {
		DMXBuffer[i] = 0;
	}

	DMXBuffer[0] = 0;
	DMXBuffer[1] = 150;

	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//printf("Sent DMX Buffer 1\n");

	sleep(500);
	DMXBuffer[0] = 127;
	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//printf("Sent DMX Buffer 2\n");

	sleep(500);
	// close
	DMXBuffer[3] = 0;
	ioctl_device(handle, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//DeviceManagerIoctl(dmx, 0, 0, &DMXBuffer[0], sizeof(DMXBuffer));
	//printf("Sent DMX Buffer 3\n");
	return 0;
}
