typedef union device_t {
	int device;
	struct {
		short driverId;
		short driverMsg;
	};
} device_t;

extern void DeviceManagerInit();
extern device_t DeviceManagerGetDevice(char* name, int len);
extern int DeviceManagerInitDevice(device_t device);
extern int DeviceManagerOpen(device_t device);
extern int DeviceManagerClose(device_t device);
extern int DeviceManagerRead(device_t device, char* buf, int len);
extern int DeviceManagerWrite(device_t device, char* buf, int len);
extern int DeviceManagerIoctl(device_t device, int msg, int mode, char* buf, int len);
