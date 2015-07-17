/*
 * devices.h
 *
 *  Created on: 04.07.2015
 *      Author: Kevin
 */

#ifndef INCLUDES_DEVICES_H_
#define INCLUDES_DEVICES_H_

extern int open_device(char* device);
extern void close_device(int handle);
extern int ioctl_device(int handle, char mode, char cmd, char* buf, int len);
extern int write_device(int handle, int len, char* buf);

#endif /* INCLUDES_DEVICES_H_ */
