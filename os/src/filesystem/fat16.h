/*
 * fat16.h
 *
 *  Created on: 15.05.2015
 *      Author: Kevin
 */

#ifndef FILESYSTEM_FAT16_H_
#define FILESYSTEM_FAT16_H_

/*
 * ATTENTION: PACK is necessary
 */

#pragma pack(push,1)
typedef struct {
    unsigned char firstByte;
    unsigned char startChs[3];
    unsigned char partitionType;
    unsigned char endChs[3];
    unsigned long startSector;
    unsigned long lengthSectors;
} PartitionTable;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sectorSize;
    unsigned char sectorsPerCluster;
    unsigned short reservedSectors;
    unsigned char numberOfFats;
    unsigned short rootDirEntries;
    unsigned short totalSectorsShort; // if zero, later field is used
    unsigned char mediaDescriptor;
    unsigned short fatSizeSectors;
    unsigned short sectorsPerTrack;
    unsigned short numberOfHeads;
    unsigned long hiddenSectors;
    unsigned long totalSectorsLong;

    unsigned char driveNumber;
    unsigned char currentHead;
    unsigned char bootSignature;
    unsigned long volumeId;
    char volumeLabel[11];
    char fsType[8];
    char bootCode[448];
    unsigned short bootSectorSignature;
} Fat16BootSector;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modifyTime;
    unsigned short modifyDate;
    unsigned short startingCluster;
    unsigned long fileSize;
} Fat16Entry;
#pragma pack(pop)

#endif
