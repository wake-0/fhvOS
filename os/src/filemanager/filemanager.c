/*
 * filemanager.c
 *
 *  Created on: 19.05.2015
 *      Author: Nicolaj Hoess
 */

#include "filemanager.h"
#include "hardcoded_programs.h"
#include "../devicemanager/devicemanager.h"
#include "../processmanager/processmanager.h"
#include "../systemapi/includes/system.h"
#include "../filesystem/ff.h"
#include "../driver/sdcard/mmcsd_proto.h"

/* Cacheline size */
#ifndef SOC_CACHELINE_SIZE
#define SOC_CACHELINE_SIZE         128
#endif

#define PATH_BUF_SIZE   					 (512)

#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
static FATFS g_sFatFs;

/* Fat devices registered */
typedef struct _fatDevice {
	/* Pointer to underlying device/controller */
	void *dev;

	/* File system pointer */
	FATFS *fs;

	/* state */
	unsigned int initDone;

} fatDevice;
extern fatDevice fat_devices[2];

static DIR g_sDirObject;
static FILINFO g_sFileInfo;
// This buffer holds the full path to the current working directory.  Initially it is root ("/").
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

static void HSMMCSDFsMount(unsigned int driveNum, void *ptr);

int FileManagerOpenExecutable(char* name, boolean_t searchInGlobalBinPath, int argc, char** argv, boolean_t blocking, context_t* context)
{
	if (searchInGlobalBinPath)
	{
		// Search in global bin path first (this includes hardcoded user-processes)
		void (*funcPtr)(int, char ** ) = HardCodedProgramsGetProgram(name);
		if (funcPtr != NULL)
		{
			process_t* proc = ProcessManagerStartProcess(name, funcPtr, blocking, context);
			return FILE_MANAGER_OK;
		}
	}

	// TODO Search in current working directory

	return FILE_MANAGER_NOT_FOUND;
}

int FileManagerInit(device_t device) {
	if (DeviceManagerInitDevice(device) != DRIVER_OK)
	{
		return FILE_MANAGER_NO_DEVICE_FOUND;
	}
	DeviceManagerOpen(device);

	int length = sizeof(mmcsdCardInfo);
	char* sdCard = (char*) malloc(length);

	if (DeviceManagerRead(device, sdCard, length) != DRIVER_OK)
	{
		return FILE_MANAGER_NO_DEVICE_FOUND;
	}

	mmcsdCardInfo* card = (mmcsdCardInfo*)(sdCard);

	// TODO: This function is not working because the &card
	HSMMCSDFsMount(0, card);

	f_mount(0, &g_sFatFs);
	fat_devices[0].dev = card;
	fat_devices[0].fs = &g_sFatFs;
	fat_devices[0].initDone = 0;

	// TODO: Remove this --> it is only for test purposes
	mmcsdCardInfo *test = (mmcsdCardInfo *) fat_devices[0].dev;

	f_opendir(&g_sDirObject, g_cCwdBuf);

	return FILE_MANAGER_OK;
}

int FileManagerListDirectoryContent(const char* name, entryType_t* buf, int length) {
	volatile unsigned int i = 0;
	volatile FRESULT fresult;

	volatile int ulTotalSize = 0;
	volatile int ulFileCount = 0;
	volatile int ulDirCount = 0;

	while (1) {
		// Read an entry from the directory.
		fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

		// Check for error and return if there is a problem.
		if (fresult != FR_OK) { return (fresult); }

		// If the file name is blank, then this is the end of the listing.
		if (!g_sFileInfo.fname[0]) { break; }

		// If the attribute is directory, then increment the directory count.
		if (g_sFileInfo.fattrib & AM_DIR) { ulDirCount++; }

		 // Otherwise, it is a file.  Increment the file count, and add in the file size to the total.
		else {
			ulFileCount++;
			ulTotalSize += g_sFileInfo.fsize;
		}

		 // Print the entry information on a single line with formatting to show
		 // the attributes, date, time, size, and name.
		printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
				(g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
				(g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
				(g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
				(g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
				(g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
				(g_sFileInfo.fdate >> 9) + 1980,
				(g_sFileInfo.fdate >> 5) & 15, g_sFileInfo.fdate & 31,
				(g_sFileInfo.ftime >> 11),
				(g_sFileInfo.ftime >> 5) & 63, g_sFileInfo.fsize,
				g_sFileInfo.fname);
	}

	printf("\n%4u File(s),%10u bytes total\n%4u Dir(s)", ulFileCount, ulTotalSize, ulDirCount);

	return FILE_MANAGER_OK;
}

int FileManagerOpenFile(const char* name, int startByte, char* buf, int length) {
	return FILE_MANAGER_OK;
}

static void HSMMCSDFsMount(unsigned int driveNum, void *ptr) {
	// TODO: this is important
	f_mount(driveNum, &g_sFatFs);
	fat_devices[driveNum].dev = ptr;
	fat_devices[driveNum].fs = &g_sFatFs;
	fat_devices[driveNum].initDone = 0;
}
