/*
 * ipcmanager.h
 *
 *  Created on: 12.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef IPC_IPCMANAGER_H_
#define IPC_IPCMANAGER_H_

#include "../platform/platform.h"

#define IPC_MANAGER_OK					(1)
#define IPC_MANAGER_ALREADY_IN_USE		(-1)
#define IPC_MANAGER_INVALID_NAME		(-2)
#define IPC_MANAGER_NOT_AUTHORISED		(-3)
#define IPC_MANAGER_NO_MESSAGE_AVAIL	(-4)

extern int IpcManagerRegisterNamespace(char* namespace_name);
extern int IpcManagerSendMessage(char* namespace_name, char* message);
extern int IpcManagerHasMessage(char* namespace_name);
extern int IpcManagerGetNextMessage(char* namespace_name, char* message_buffer, int len);

#endif /* IPC_IPCMANAGER_H_ */
