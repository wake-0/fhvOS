/*
 * devicemanager.c
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#include "devicemanager.h"

#include "../hal/platform/platform.h"

#define MAX_DEVICES		32
#define MAX_DEVICE_NAME	255

typedef struct device_map_entry_t {
	char name[MAX_DEVICE_NAME];
	driver_id_t driverId;
	short driverMsg;
} device_map_entry_t;

static tBoolean compareEntry(device_map_entry_t* entry, char* name, int len);

/*
 * This is a mapping between device names and the driver for that device.
 * All device names have to be \0-terminated.
 */
static device_map_entry_t deviceMap[MAX_DEVICES] = {
		{ "LED0\0", DRIVER_ID_LED, 0 },
		{ "LED1\0", DRIVER_ID_LED, 1 },
		{ "LED2\0", DRIVER_ID_LED, 2 },
		{ "LED3\0", DRIVER_ID_LED, 3 }
};

void DeviceManagerInit()
{
	DriverManagerInit();
}

device_t DeviceManagerGetDevice(char* name, int len)
{
	int i = 0;
	for (i = 0; i < MAX_DEVICES; i++) {
		device_map_entry_t dev = deviceMap[0];
		if (dev.name[0] != '\0') {
			if (compareEntry(&dev, name, len))
			{
				device_t result;
				result.driverId = dev.driverId;
				result.driverMsg = dev.driverMsg;
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

tBoolean compareEntry(device_map_entry_t* entry, char* name, int len)
{
	int cnt = 0;
	tBoolean equal = true;
	while (entry->name[cnt] != '\0' && cnt <= len) {
		if (entry->name[cnt] != name[cnt]) equal = false;
		cnt++;
	}
	return equal && (cnt == len);
}

