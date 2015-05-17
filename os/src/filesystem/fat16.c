/*
 * fat16.c
 *
 *  Created on: 15.05.2015
 *      Author: Kevin
 */
#include "fat16.h"
#include "filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kernel/kernel.h"

// general defines
#define BUFFER_SIZE 			(4096)
#define END_OF_FILE				(0xFFFF)

// filetype defines
#define ENTRY_UNUSED			(0x00)
#define ENTRY_DELETED			(0xE5)
#define ENTRY_STARTING_0xE5		(0x05)
#define ENTRY_DIRECTORY			(0x2E)

// Forward declarations
static void printFileInfo(Fat16Entry* entry);
static void readFile(FILE * in, FILE * out, unsigned long fatStart,
		unsigned long dataStart, unsigned long clusterSize,
		unsigned short cluster, unsigned long fileSize);

static void printFileInfo(Fat16Entry* entry) {
	switch (entry->filename[0]) {
	case ENTRY_UNUSED:
		return; // unused entry
	case ENTRY_DELETED:
		KernelDebug("Deleted file: [?%.7s.%.3s]\n", entry->filename + 1, entry->ext);
		return;
	case ENTRY_STARTING_0xE5:
		KernelDebug("File starting with 0xE5: [%c%.7s.%.3s]\n", 0xE5,
				entry->filename + 1, entry->ext);
		break;
	case ENTRY_DIRECTORY:
		KernelDebug("Directory: [%.8s.%.3s]\n", entry->filename, entry->ext);
		break;
	default:
		KernelDebug("File: [%.8s.%.3s]\n", entry->filename, entry->ext);
	}

	KernelDebug(
			"  Modified: %04d-%02d-%02d %02d:%02d.%02d    Start: [%04X]    Size: %d\n",
			1980 + (entry->modifyDate >> 9), (entry->modifyDate >> 5) & 0xF,
			entry->modifyDate & 0x1F, (entry->modifyTime >> 11),
			(entry->modifyTime >> 5) & 0x3F, entry->modifyTime & 0x1F,
			entry->startingCluster, entry->fileSize);
}

static void readFile(FILE * in, FILE * out, unsigned long fatStart,
		unsigned long dataStart, unsigned long clusterSize,
		unsigned short cluster, unsigned long fileSize) {

	unsigned char buffer[BUFFER_SIZE];
	size_t bytesRead;
	size_t bytesToRead;
	size_t fileLeft = fileSize;
	size_t clusterLeft = clusterSize;

	// Go to first data cluster
	fseek(in, dataStart + clusterSize * (cluster - 2), SEEK_SET);

	// Read until we run out of file or clusters
	while (fileLeft > 0 && cluster != END_OF_FILE) {
		bytesToRead = sizeof(buffer);

		// don't read past the file or cluster end
		if (bytesToRead > fileLeft) {
			bytesToRead = fileLeft;
		}
		if (bytesToRead > clusterLeft) {
			bytesToRead = clusterLeft;
		}

		// read data from cluster, write to file
		bytesRead = fread(buffer, 1, bytesToRead, in);
		fwrite(buffer, 1, bytesRead, out);
		KernelDebug("Copied %d bytes\n", bytesRead);

		// decrease byte counters for current cluster and whole file
		clusterLeft -= bytesRead;
		fileLeft -= bytesRead;

		// if we have read the whole cluster, read next cluster # from FAT
		if (clusterLeft == 0) {
			fseek(in, fatStart + cluster * 2, SEEK_SET);
			fread(&cluster, 2, 1, in);

			KernelDebug("End of cluster reached, next cluster %d\n", cluster);

			fseek(in, dataStart + clusterSize * (cluster - 2), SEEK_SET);
			clusterLeft = clusterSize; // reset cluster byte counter
		}
	}
}

int Fat16PrintFilesFromRootDirectory(void) {
	// TODO: fix string for fopen
	FILE * in = fopen("test.txt", "rb");
	PartitionTable pt[4];
	Fat16BootSector bs;
	Fat16Entry entry;

	// go to partition table start
	fseek(in, 0x1BE, SEEK_SET);
	// read all four entries
	fread(pt, sizeof(PartitionTable), 4, in);

	// search for partition table
	int i;
	for (i = 0; i < 4; i++) {
		if (pt[i].partitionType == 4 || pt[i].partitionType == 6
				|| pt[i].partitionType == 14) {
			KernelDebug("FAT16 filesystem found from partition %d\n", i);
			break;
		}
	}

	if (i == 4) {
		KernelDebug("No FAT16 filesystem found, exiting...\n");
		return -1;
	}

	fseek(in, 512 * pt[i].startSector, SEEK_SET);
	fread(&bs, sizeof(Fat16BootSector), 1, in);

	KernelDebug("Now at 0x%X, sector size %d, FAT size %d sectors, %d FATs\n\n",
			ftell(in), bs.sectorSize, bs.fatSizeSectors, bs.numberOfFats);

	fseek(in,
			(bs.reservedSectors - 1 + bs.fatSizeSectors * bs.numberOfFats)
					* bs.sectorSize, SEEK_CUR);

	for (i = 0; i < bs.rootDirEntries; i++) {
		fread(&entry, sizeof(entry), 1, in);
		printFileInfo(&entry);
	}

	KernelDebug("\nRoot directory read, now at 0x%X\n", ftell(in));
	fclose(in);

	return 0;
}

int Fat16ReadFile(char* fileSystem, char* fileToRead) {
	FILE* in;
	FILE* out;

	unsigned long fatStart;
	unsigned long rootStart;
	unsigned long dataStart;

	PartitionTable pt[4];
	Fat16BootSector bs;
	Fat16Entry entry;

	// initially pad with spaces
	char filename[9] = "        ";
	char fileExt[4] = "   ";

	if ((in = fopen(fileSystem, "rb")) == NULL) {
		KernelError("Filesystem image file %s not found!\n", fileSystem);
		return -1;
	}

	// Copy filename and extension to space-padded search strings
	int i;
	for (i = 0; i < 8 && fileToRead[i] != '.' && fileToRead[i] != 0; i++) {
		filename[i] = fileToRead[i];
	}

	int j;
	for (j = 1; j <= 3 && fileToRead[i + j] != 0; j++) {
		fileExt[j - 1] = fileToRead[i + j];
	}

	KernelDebug("Opened %s, looking for [%s.%s]\n", fileSystem, filename, fileExt);

	// go to partition table start
	fseek(in, 0x1BE, SEEK_SET);
	// read all four entries
	fread(pt, sizeof(PartitionTable), 4, in);

	for (i = 0; i < 4; i++) {
		if (pt[i].partitionType == 4 || pt[i].partitionType == 6
				|| pt[i].partitionType == 14) {
			KernelDebug("FAT16 filesystem found from partition %d\n", i);
			break;
		}
	}

	if (i == 4) {
		KernelError("No FAT16 filesystem found, exiting...\n");
		return -1;
	}

	fseek(in, 512 * pt[i].startSector, SEEK_SET);
	fread(&bs, sizeof(Fat16BootSector), 1, in);

	// Calculate start offsets of FAT, root directory and data
	fatStart = ftell(in) + (bs.reservedSectors - 1) * bs.sectorSize;
	rootStart = fatStart + bs.fatSizeSectors * bs.numberOfFats * bs.sectorSize;
	dataStart = rootStart + bs.rootDirEntries * sizeof(Fat16Entry);

	KernelDebug("FAT start at %08X, root dir at %08X, data at %08X\n", fatStart,
			rootStart, dataStart);

	fseek(in, rootStart, SEEK_SET);

	for (i = 0; i < bs.rootDirEntries; i++) {
		fread(&entry, sizeof(entry), 1, in);

		if (memcmp(entry.filename, filename, 8) == 0
				&& memcmp(entry.ext, fileExt, 3) == 0) {
			KernelDebug("File found!\n");
			break;
		}
	}

	if (i == bs.rootDirEntries) {
		KernelError("File not found!");
		return -1;
	}

	// write the file contents to disk
	out = fopen(fileToRead, "wb");
	readFile(in, out, fatStart, dataStart, bs.sectorsPerCluster * bs.sectorSize,
			entry.startingCluster, entry.fileSize);

	fclose(out);
	fclose(in);

	return 0;
}
