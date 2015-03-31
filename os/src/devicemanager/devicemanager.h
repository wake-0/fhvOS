/*
 * devicemanager.h
 *
 *  Created on: 17.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef DEVICEMANAGER_DEVICEMANAGER_H_
#define DEVICEMANAGER_DEVICEMANAGER_H_

#include "../../../platform/platform.h"
#include "../driver/manager/driver_manager.h"

#define DEVICE_MANAGER_NO_DRIVER	-10

typedef union device_t {
	int device;
	struct {
		short driverId;
		short driverMsg;
	};
} device_t;

extern void DeviceManagerInit();
extern device_t DeviceManagerGetDevice(char* name, int len);

extern int DeviceManagerOpen(device_t device);
extern int DeviceManagerClose(device_t device);
extern int DeviceManagerRead(device_t device, char* buf, int len);
extern int DeviceManagerWrite(device_t device, char* buf, int len);
extern int DeviceManagerIoctl(device_t device, int msg, int mode, char* buf, int len);

#endif /* DEVICEMANAGER_DEVICEMANAGER_H_ */
