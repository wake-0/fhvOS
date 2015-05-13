/*
 * fat16.h
 *
 *  Created on: 13.05.2015
 *      Author: Kevin
 */

#ifndef FAT16__SRC_FAT16_H_
#define FAT16__SRC_FAT16_H_

/*
 * ATTENTION: PACK is necessary
 */

#pragma pack(push,1)
typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned long start_sector;
    unsigned long length_sectors;
} PartitionTable;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short; // if zero, later field is used
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned long hidden_sectors;
    unsigned long total_sectors_long;

    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned long volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
} Fat16BootSector;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short starting_cluster;
    unsigned long file_size;
} Fat16Entry;
#pragma pack(pop)

#endif /* FAT16__SRC_FAT16_H_ */
