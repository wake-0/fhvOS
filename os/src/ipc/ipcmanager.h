/*
 * ipcmanager.h
 *
 *  Created on: 12.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef IPC_IPCMANAGER_H_
#define IPC_IPCMANAGER_H_

#include "../platform/platform.h"
#include <ipc.h>

#define IPC_MANAGER_OK					(1)
#define IPC_MANAGER_ALREADY_IN_USE		(-1)
#define IPC_MANAGER_INVALID_NAME		(-2)
#define IPC_MANAGER_NOT_AUTHORISED		(-3)
#define IPC_MANAGER_NO_MESSAGE_AVAIL	(-4)
#define IPC_MANAGER_MESSAGE_TOO_LONG	(-5)
#define IPC_MANAGER_RECEIVER_BUSY		(-6)
#define IPC_MANAGER_BUFFER_TOO_SMALL	(-7)
#define IPC_MANAGER_UKNOWN_ERROR		(-10)

extern int IpcManagerRegisterNamespace(char* namespace_name);
extern int IpcManagerSendMessage(char* sender_namespace, char* namespace_name, char* message, int len);
extern int IpcManagerHasMessage(char* namespace_name);
extern int IpcManagerGetNextMessage(char* namespace_name, char* message_buffer, int msg_buf_len, char* sender_namespace, int ns_buf_len);
extern int IpcManagerCloseNamespace(char* namespace_name);
extern int IpcManagerChannelCount();
extern int IpcManagerGetChannel(int index, char* buf, int len);

#endif /* IPC_IPCMANAGER_H_ */
