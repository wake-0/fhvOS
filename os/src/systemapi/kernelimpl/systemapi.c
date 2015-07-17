/*
 * systemcallFunctions.c
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#include "systemapi.h"
#include "../../hal/cpu/hal_cpu.h"
#include "../../kernel/kernel.h"
#include "../../scheduler/scheduler.h"
#include "../../filemanager/filemanager.h"
#include "../../processmanager/processmanager.h"
#include "../../ipc/ipcmanager.h"
#include <stdio.h>
#include <string.h>
#include <ipc.h>

/**
 * \brief	Handles system calls. This function is called by the assembler swi_handler function
 * 			in interrupt.asm.
 */
void SystemCallHandler(systemCallMessage_t* message, unsigned int systemCallNumber, context_t* context)
{
	// We allow interrupts but disallow scheduling
	KernelAtomicStart();

	//KernelDebug("Systemcall number=%i\n", message->systemCallNumber);
	switch (message->systemCallNumber)
	{
		case SYSTEM_CALL_EXEC:
		{
			CPUirqe();
			// Disassemble argument package
			char* command = message->messageArgs.callBuf;
			KernelExecute(command, context);
			break;
		}
		case SYSTEM_CALL_YIELD:
		{
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_EXIT:
		{
			int value = message->messageArgs.callArg;
			(void)(value); // Get rid of unused warning TODO Use the return value of the proc
			process_t* curr = SchedulerGetRunningProcess();
			ProcessManagerKillProcess(curr->id);
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_SLEEP:
		{
			int millis = (unsigned int) message->messageArgs.callArg;
			process_t* curr = SchedulerGetRunningProcess();
			SchedulerSleepProcess(curr->id, millis);
			SchedulerRunNextProcess(context);
			break;
		}
		case SYSTEM_CALL_TIME:
		{
			long millis = KernelGetUptime();
			memcpy(message->messageArgs.returnBuf, &millis, sizeof(long));
			break;
		}
		case SYSTEM_CALL_CWD:
		{
			CPUirqe();
			int res = FileManagerGetCurrentWorkingDirectory(message->messageArgs.returnBuf, message->messageArgs.callArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_READ:
		{
			CPUirqe();
			int res = FileManagerOpenFile(message->messageArgs.callBuf, message->messageArgs.callArg, message->messageArgs.returnBuf, *message->messageArgs.returnArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_READ_DIR:
		{
			CPUirqe();
			int res = FileManagerListDirectoryContent(message->messageArgs.callBuf, (directoryEntry_t*)(message->messageArgs.returnBuf), *message->messageArgs.returnArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_CHDIR:
		{
			CPUirqe();
			int res = FileManagerSetCurrentWorkingDirectory(message->messageArgs.callBuf);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_GET_PROC_COUNT:
		{
			int res = ProcessManagerGetRunningProcessesCount();
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_GET_PROC_LIST:
		{
			int res = ProcessManagerListProcesses((processInfoAPI_t*) message->messageArgs.returnBuf, message->messageArgs.callArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_KILL:
		{
			int res = ProcessManagerKillProcess(message->messageArgs.callArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_PRINT:
			printf("%*.*s", message->messageArgs.callArg, message->messageArgs.callArg, message->messageArgs.callBuf);
			break;
		case SYSTEM_CALL_OPEN_DEVICE:
		{
			device_t device = DeviceManagerGetDevice(message->messageArgs.callBuf, strlen(message->messageArgs.callBuf));
			DeviceManagerOpen(device);
			*message->messageArgs.returnArg = device.device;
			break;
		}
		case SYSTEM_CALL_CLOSE_DEVICE:
		{
			device_t device;
			device.device = message->messageArgs.callArg;
			int res = DeviceManagerClose(device);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_IOCTL_DEVICE:
		{
			device_t device;
			int cmd = message->messageArgs.callArg4;
			int mode = message->messageArgs.callArg3;
			device.device = message->messageArgs.callArg2;
			char* buf = message->messageArgs.callBuf;
			device.device = *message->messageArgs.returnArg;
			int res = DeviceManagerIoctl(device, cmd, mode, buf, message->messageArgs.callArg );
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_WRITE_DEVICE:
		{
			device_t device;
			device.device = *message->messageArgs.returnArg;
			int res = DeviceManagerWrite(device, message->messageArgs.callBuf, message->messageArgs.callArg);
			*message->messageArgs.returnArg = res;
			break;
		}
		case SYSTEM_CALL_IPC_OPEN:
		{
			*message->messageArgs.returnArg = IpcManagerRegisterNamespace(message->messageArgs.callBuf);
			break;
		}
		case SYSTEM_CALL_IPC_CLOSE:
		{
			*message->messageArgs.returnArg = IpcManagerCloseNamespace(message->messageArgs.callBuf);
			break;
		}
		case SYSTEM_CALL_IPC_SEND:
		{
			char* buf = message->messageArgs.callBuf;
			int len_sender = buf[0];
			int len_receiver = buf[1];
			(void)(len_receiver);
			*message->messageArgs.returnArg = IpcManagerSendMessage(&message->messageArgs.callBuf[2], &message->messageArgs.callBuf[2 + len_sender + 1], message->messageArgs.returnBuf, *message->messageArgs.returnArg);
			break;
		}
		case SYSTEM_CALL_IPC_GET:
		{
			char* buf = message->messageArgs.returnBuf;
			int len_message = buf[0];
			int len_sender = buf[1];
			*message->messageArgs.returnArg = IpcManagerGetNextMessage(message->messageArgs.callBuf, &buf[2], len_message, &buf[2 + len_message], len_sender);
			break;
		}
		case SYSTEM_CALL_IPC_CHECK:
		{
			*message->messageArgs.returnArg = IpcManagerHasMessage(message->messageArgs.callBuf);
			break;
		}
		case SYSTEM_CALL_IPC_LIST:
		{
			int count = IpcManagerChannelCount();
			ipc_channel_list_entry_t* buffer = (ipc_channel_list_entry_t*) message->messageArgs.callBuf;
			int i = 0;
			for (i = 0; i < count && i < message->messageArgs.callArg; i++)
			{
				IpcManagerGetChannel(i, buffer[i].channel_name, IPC_MAX_NAMESPACE_NAME);
			}
			*message->messageArgs.returnArg = i;
			break;
		}
	}
	KernelAtomicEnd();
}
