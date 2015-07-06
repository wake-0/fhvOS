/*
 * ipc.h
 *
 *  Created on: 05.07.2015
 *      Author: Nicolaj Hoess
 */

#ifndef INCLUDES_IPC_H_
#define INCLUDES_IPC_H_

extern int open_ipc_channel(char* namespace_name);
extern int close_ipc_channel(char* namespace_name);
extern int send_ipc_message(char* sender, char* receiver, char* message, int len);
extern int has_ipc_message(char* namespace_name);
extern int get_next_ipc_message(char* namespace_name, char* messageBuf, int len, char* sender_buf, int sender_len);

#endif /* INCLUDES_IPC_H_ */
