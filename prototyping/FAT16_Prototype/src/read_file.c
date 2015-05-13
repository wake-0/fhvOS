#include <stdio.h>
#include <stdlib.h>
#include "fat16.h"

static void printFileInfo(Fat16Entry *entry) {
    switch(entry->filename[0]) {
    case 0x00:
        return; // unused entry
    case 0xE5:
        printf("Deleted file: [?%.7s.%.3s]\n", entry->filename+1, entry->ext);
        return;
    case 0x05:
        printf("File starting with 0xE5: [%c%.7s.%.3s]\n", 0xE5, entry->filename+1, entry->ext);
        break;
    case 0x2E:
        printf("Directory: [%.8s.%.3s]\n", entry->filename, entry->ext);
        break;
    default:
        printf("File: [%.8s.%.3s]\n", entry->filename, entry->ext);
    }

    printf("  Modified: %04d-%02d-%02d %02d:%02d.%02d    Start: [%04X]    Size: %d\n",
        1980 + (entry->modify_date >> 9), (entry->modify_date >> 5) & 0xF, entry->modify_date & 0x1F,
        (entry->modify_time >> 11), (entry->modify_time >> 5) & 0x3F, entry->modify_time & 0x1F,
        entry->starting_cluster, entry->file_size);
}

static void readFile(FILE * in, FILE * out,
                   unsigned long fat_start,
                   unsigned long data_start,
                   unsigned long cluster_size,
                   unsigned short cluster,
                   unsigned long file_size) {
    unsigned char buffer[4096];
    size_t bytes_read, bytes_to_read,
           file_left = file_size, cluster_left = cluster_size;

    // Go to first data cluster
    fseek(in, data_start + cluster_size * (cluster-2), SEEK_SET);

    // Read until we run out of file or clusters
    while(file_left > 0 && cluster != 0xFFFF) {
        bytes_to_read = sizeof(buffer);

        // don't read past the file or cluster end
        if(bytes_to_read > file_left)
            bytes_to_read = file_left;
        if(bytes_to_read > cluster_left)
            bytes_to_read = cluster_left;

        // read data from cluster, write to file
        bytes_read = fread(buffer, 1, bytes_to_read, in);
        fwrite(buffer, 1, bytes_read, out);
        printf("Copied %d bytes\n", bytes_read);

        // decrease byte counters for current cluster and whole file
        cluster_left -= bytes_read;
        file_left -= bytes_read;

        // if we have read the whole cluster, read next cluster # from FAT
        if(cluster_left == 0) {
            fseek(in, fat_start + cluster*2, SEEK_SET);
            fread(&cluster, 2, 1, in);

            printf("End of cluster reached, next cluster %d\n", cluster);

            fseek(in, data_start + cluster_size * (cluster-2), SEEK_SET);
            cluster_left = cluster_size; // reset cluster byte counter
        }
    }
}

int fatPrintFilesFromRootDirectory(void) {
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
		if (pt[i].partition_type == 4 || pt[i].partition_type == 6 || pt[i].partition_type == 14) {
			printf("FAT16 filesystem found from partition %d\n", i);
			break;
		}
	}

	if (i == 4) {
		printf("No FAT16 filesystem found, exiting...\n");
		return -1;
	}

	fseek(in, 512 * pt[i].start_sector, SEEK_SET);
	fread(&bs, sizeof(Fat16BootSector), 1, in);

	printf("Now at 0x%X, sector size %d, FAT size %d sectors, %d FATs\n\n", ftell(in), bs.sector_size, bs.fat_size_sectors, bs.number_of_fats);

	fseek(in, (bs.reserved_sectors - 1 + bs.fat_size_sectors * bs.number_of_fats) * bs.sector_size, SEEK_CUR);

	for (i = 0; i < bs.root_dir_entries; i++) {
		fread(&entry, sizeof(entry), 1, in);
		printFileInfo(&entry);
	}

	printf("\nRoot directory read, now at 0x%X\n", ftell(in));
	fclose(in);

	return 0;
}

int fatReadFile(char* fileSystem, char* fileToRead) {
	 FILE *in, *out;
	    int i, j;
	    unsigned long fat_start, root_start, data_start;
	    PartitionTable pt[4];
	    Fat16BootSector bs;
	    Fat16Entry entry;
		char filename[9] = "        ", file_ext[4] = "   "; // initially pad with spaces

		if((in = fopen(fileSystem, "rb")) == NULL) {
			printf("Filesystem image file %s not found!\n", fileSystem);
			return -1;
		}

	    // Copy filename and extension to space-padded search strings
		for(i=0; i<8 && fileToRead[i] != '.' && fileToRead[i] != 0; i++)
			filename[i] = fileToRead[i];
		for(j=1; j<=3 && fileToRead[i+j] != 0; j++)
			file_ext[j-1] = fileToRead[i+j];

		printf("Opened %s, looking for [%s.%s]\n", fileSystem, filename, file_ext);

	    fseek(in, 0x1BE, SEEK_SET); // go to partition table start
	    fread(pt, sizeof(PartitionTable), 4, in); // read all four entries

	    for(i=0; i<4; i++) {
	        if(pt[i].partition_type == 4 || pt[i].partition_type == 6 ||
	           pt[i].partition_type == 14) {
	            printf("FAT16 filesystem found from partition %d\n", i);
	            break;
	        }
	    }

	    if(i == 4) {
	        printf("No FAT16 filesystem found, exiting...\n");
	        return -1;
	    }

	    fseek(in, 512 * pt[i].start_sector, SEEK_SET);
	    fread(&bs, sizeof(Fat16BootSector), 1, in);

	    // Calculate start offsets of FAT, root directory and data
	    fat_start = ftell(in) + (bs.reserved_sectors-1) * bs.sector_size;
	    root_start = fat_start + bs.fat_size_sectors * bs.number_of_fats *
	        bs.sector_size;
	    data_start = root_start + bs.root_dir_entries * sizeof(Fat16Entry);

	    printf("FAT start at %08X, root dir at %08X, data at %08X\n",
	           fat_start, root_start, data_start);

	    fseek(in, root_start, SEEK_SET);

	    for(i=0; i<bs.root_dir_entries; i++) {
	        fread(&entry, sizeof(entry), 1, in);

			if(memcmp(entry.filename, filename, 8) == 0 &&
			   memcmp(entry.ext, file_ext, 3) == 0) {
	            printf("File found!\n");
	            break;
	        }
	    }

	    if(i == bs.root_dir_entries) {
	        printf("File not found!");
	        return -1;
	    }

	    out = fopen(fileToRead, "wb"); // write the file contents to disk
	    readFile(in, out, fat_start, data_start, bs.sectors_per_cluster *
	                  bs.sector_size, entry.starting_cluster, entry.file_size);
	    fclose(out);

	    fclose(in);

	    return 0;
}

int main() {
	int returnValue = 0;
	// returnValue = fatPrintFilesFromRootDirectory();
	returnValue = fatReadFile("test.img", "README.TXT");
    return returnValue;
}
