/*
 * devicemanager.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "devicemanager.h"

#define MAX_DEVICES		32
#define MAX_DEVICE_NAME	255

typedef struct {
	char name[MAX_DEVICE_NAME];
	driver_id_t driverId;
	short driverMsg;
	boolean_t initialized;
} device_map_entry_t;

static boolean_t compareEntry(device_map_entry_t* entry, char* name, int len);

/*
 * This is a mapping between device names and the driver for that device.
 * All device names have to be \0-terminated.
 */
static device_map_entry_t deviceMap[MAX_DEVICES] = {
		{ "LED0", DRIVER_ID_LED, 0, false },
		{ "LED1", DRIVER_ID_LED, 1, false },
		{ "LED2", DRIVER_ID_LED, 2, false },
		{ "LED3", DRIVER_ID_LED, 3, false },
		{ "UART0", DRIVER_ID_UART, 0, false },
		{ "UART1", DRIVER_ID_UART, 1, false },
		{ "UART2", DRIVER_ID_UART, 2, false },
		{ "UART3", DRIVER_ID_UART, 4, false },
		{ "UART4", DRIVER_ID_UART, 5, false }

};

void DeviceManagerInit()
{
	DriverManagerInit();
}

device_t DeviceManagerGetDevice(char* name, int len)
{
	int i = 0;
	for (i = 0; i < MAX_DEVICES; i++) {
		device_map_entry_t dev = deviceMap[i];
		if (dev.name[0] != '\0') {
			if (compareEntry(&dev, name, len))
			{
				device_t result;
				result.driverId = dev.driverId;
				result.driverMsg = dev.driverMsg;

				// Initialize the driver if we return it for the first time
				if (dev.initialized == false) {
					DriverManagerGetDriver(dev.driverId)->init(dev.driverMsg);
					dev.initialized = true;
				}
				return result;
			}
		}
	}
	return (device_t) 0;
}

int DeviceManagerOpen(device_t device)
{
	driver_t* driver = DriverManagerGetDriver(device.driverId);
	if (driver == NULL) return DEVICE_MANAGER_NO_DRIVER;
	return driver->open(device.driverMsg);
}

int DeviceManagerClose(device_t device)
{
	driver_t* driver = DriverManagerGetDriver(device.driverId);
	if (driver == NULL) return DEVICE_MANAGER_NO_DRIVER;
	return driver->close(device.driverMsg);
}

int DeviceManagerRead(device_t device, char* buf, int len)
{
	driver_t* driver = DriverManagerGetDriver(device.driverId);
	if (driver == NULL) return DEVICE_MANAGER_NO_DRIVER;
	return driver->read(device.driverMsg, buf, len);
}

int DeviceManagerWrite(device_t device, char* buf, int len)
{
	driver_t* driver = DriverManagerGetDriver(device.driverId);
	if (driver == NULL) return DEVICE_MANAGER_NO_DRIVER;
	return driver->write(device.driverMsg, buf, len);
}

int DeviceManagerIoctl(device_t device, int msg, int mode, char* buf, int len)
{
	driver_t* driver = DriverManagerGetDriver(device.driverId);
	if (driver == NULL) return DEVICE_MANAGER_NO_DRIVER;
	return driver->ioctl(device.driverMsg, msg, mode, buf, len);
}

boolean_t compareEntry(device_map_entry_t* entry, char* name, int len)
{
	int cnt = 0;
	boolean_t equal = true;
	while (entry->name[cnt] != '\0' && cnt <= len) {
		if (entry->name[cnt] != name[cnt]) equal = false;
		cnt++;
	}
	return equal && (cnt == len);
}

