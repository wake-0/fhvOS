/*
 * ipcmanager.c
 *
 *  Created on: 12.05.2015
 *      Author: Nicolaj Hoess
 */

#include "ipcmanager.h"
#include <string.h>
#include <stdlib.h>
#include "../scheduler/scheduler.h"

#define MAX_OPEN_DOMAINS			(100)
#define MAX_MESSAGE_LENGTH			(100)
#define MAX_MESSAGE_INBOX			(100)

typedef struct {
	char* message;
	int msg_len;
	char* sender_namespace;
} ipc_message_t;

typedef struct {
	int procId;
	char* name;
	ipc_message_t* messageQueue[MAX_MESSAGE_INBOX];
	unsigned int messageCount;
} namespace_t;

static namespace_t* namespaces[MAX_OPEN_DOMAINS];
static volatile int nsIdx = 0;
namespace_t* getNamespaceByName(char* name);

int IpcManagerRegisterNamespace(char* namespace_name)
{
	// TODO Mutex
	if (nsIdx >= (MAX_OPEN_DOMAINS))
	{
		return IPC_MANAGER_UKNOWN_ERROR;
	}
	if (getNamespaceByName(namespace_name) != NULL)
	{
		return IPC_MANAGER_ALREADY_IN_USE;
	}

	process_t* proc = SchedulerGetRunningProcess();
	if (proc == NULL)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}
	size_t len = strlen(namespace_name);
	if (len > IPC_MAX_NAMESPACE_NAME)
	{
		return IPC_MANAGER_INVALID_NAME;
	}

	// Create new namespace
	namespace_t* ns = malloc(sizeof(namespace_t));

	// Allocate space for the name
	char* charptr = malloc((len + 1) * sizeof(char));
	ns->name = charptr;
	strcpy(ns->name, namespace_name);
	ns->name[len] = '\0';

	ns->procId = proc->id;
	ns->messageCount = 0;
	namespaces[nsIdx++] = ns;
	return IPC_MANAGER_OK;
}

int IpcManagerSendMessage(char* sender_namespace, char* namespace_name, char* message, int msg_len)
{
	// Get sender
	namespace_t* sender = getNamespaceByName(sender_namespace);
	if (sender == NULL)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}

	process_t* proc = SchedulerGetRunningProcess();
	if (proc->id != sender->procId)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}

	// Get receiver
	namespace_t* receiver = getNamespaceByName(namespace_name);
	if (receiver == NULL)
	{
		return IPC_MANAGER_INVALID_NAME;
	}

	if (msg_len > MAX_MESSAGE_LENGTH)
	{
		return IPC_MANAGER_MESSAGE_TOO_LONG;
	}

	if (receiver->messageCount >= MAX_MESSAGE_INBOX)
	{
		return IPC_MANAGER_RECEIVER_BUSY;
	}

	// Add new message
	ipc_message_t* newMessage = malloc(sizeof(ipc_message_t));
	newMessage->sender_namespace = sender->name;
	newMessage->msg_len = msg_len;
	newMessage->message = malloc((msg_len) * sizeof(char));
	memcpy(newMessage->message, message, msg_len);

	// TODO Add a mutex/semaphore
	receiver->messageQueue[receiver->messageCount++] = newMessage;

	return IPC_MANAGER_OK;
}

int IpcManagerHasMessage(char* namespace_name)
{
	namespace_t* receiver = getNamespaceByName(namespace_name);
	if (receiver == NULL)
	{
		return IPC_MANAGER_INVALID_NAME;
	}
	process_t* proc = SchedulerGetRunningProcess();
	if (proc->id != receiver->procId)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}

	return receiver->messageCount;
}

int IpcManagerGetNextMessage(char* namespace_name, char* message_buffer, int msg_buf_len, char* sender_namespace, int sender_ns_len)
{
	namespace_t* receiver = getNamespaceByName(namespace_name);
	if (receiver == NULL)
	{
		return IPC_MANAGER_INVALID_NAME;
	}
	process_t* proc = SchedulerGetRunningProcess();
	if (proc->id != receiver->procId)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}

	if (receiver->messageCount <= 0)
	{
		return IPC_MANAGER_NO_MESSAGE_AVAIL;
	}
	// TODO Mutex

	ipc_message_t* message = receiver->messageQueue[0];

	if (msg_buf_len < message->msg_len)
	{
		return IPC_MANAGER_BUFFER_TOO_SMALL;
	}

	if (sender_ns_len < strlen(message->sender_namespace) + 1)
	{
		return IPC_MANAGER_BUFFER_TOO_SMALL;
	}

	memcpy(message_buffer, message->message, message->msg_len);
	strcpy(sender_namespace, message->sender_namespace);

	// Remove message from queue and shift entries
	int idx = 0;
	for (idx = 0; idx < receiver->messageCount - 1; idx++)
	{
		receiver->messageQueue[idx] = receiver->messageQueue[idx+1];
	}
	receiver->messageCount--;

	// Free memory
	free(message->message);
	free(message);

	return IPC_MANAGER_OK;
}

int IpcManagerCloseNamespace(char* namespace_name)
{
	namespace_t* ns = getNamespaceByName(namespace_name);
	if (ns == NULL)
	{
		return IPC_MANAGER_INVALID_NAME;
	}

	process_t* proc = SchedulerGetRunningProcess();
	if (proc == NULL || proc->id != ns->procId)
	{
		return IPC_MANAGER_NOT_AUTHORISED;
	}

	// Delete from array
	int cnt = 0;
	while (namespaces[cnt] != ns) { cnt++; }
	free(ns);
	namespaces[cnt] = NULL;
	while ( (cnt + 1) < MAX_OPEN_DOMAINS && namespaces[cnt+1] != NULL)
	{
		namespaces[cnt] = namespaces[cnt + 1];
		cnt++;
	}

	namespaces[--nsIdx] = NULL;
	return IPC_MANAGER_OK;
}

int IpcManagerChannelCount()
{
	return nsIdx;
}

int IpcManagerGetChannel(int index, char* buf, int len)
{
	if (index > nsIdx)
	{
		return IPC_MANAGER_INVALID_NAME;
	}
	int name_len = strlen(namespaces[index]->name);
	if (len < name_len + 1)
	{
		return IPC_MANAGER_BUFFER_TOO_SMALL;
	}
	memcpy(buf, namespaces[index]->name, name_len);
	buf[name_len] = '\0';
	return IPC_MANAGER_OK;
}


namespace_t* getNamespaceByName(char* name)
{
	int idx = 0;
	while (namespaces[idx] != NULL)
	{
		int res = strcmp(namespaces[idx]->name, name);
		if (res == 0)
		{
			return namespaces[idx];
		}
		idx++;
	}
	return NULL;
}
