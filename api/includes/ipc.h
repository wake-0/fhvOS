/*
 * ipc.h
 *
 *  Created on: 05.07.2015
 *      Author: Nicolaj Hoess
 */

#ifndef INCLUDES_IPC_H_
#define INCLUDES_IPC_H_

#define IPC_MAX_NAMESPACE_NAME			(50)

typedef struct {
	char channel_name[IPC_MAX_NAMESPACE_NAME];
} ipc_channel_list_entry_t;

extern int open_ipc_channel(char* namespace_name);
extern int close_ipc_channel(char* namespace_name);
extern int send_ipc_message(char* sender, char* receiver, char* message, int len);
extern int has_ipc_message(char* namespace_name);
extern int get_next_ipc_message(char* namespace_name, char* messageBuf, int len, char* sender_buf, int sender_len);
extern int list_ipc_channels(ipc_channel_list_entry_t* buffer, int len);

#endif /* INCLUDES_IPC_H_ */
