// Nate Loken
// CS-474 Operating Systems
// This program takes one command line parameter, the disk image file to be analyzed.
/*
    Once the file is inputed the raw data is read using the open and read functions, with this the data stored in the first 
    512 bytes where the Master Boot Record is most likely to be found, is stored in a new MASTER_BOOT_RECORD struct variable.
    Next it is determined whether or not the file is NTFS or not using the partition labels found from the mbr, and if it is NTFS
    then then a new NTFS variable is created and the data from the bootstrap code is inputed into the variable and the information regarding the 
    file system is printed. This includes the sector size, sectors per track, number of heads, etc.
    However, if the file isn't of NTFS then it checks to see if the file is EXT4 and if it is then an EXT4 variable is created to hold the data
    regarding the inodes, blocks, uuid etc. Then this data is printed out to the user.

    **Note: This program assumes that ext4 file systems don't use the mbr space at the begining of the file so that area is free, this means that files
            who are empty will be misidentified.
*/
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "mbr.h"
#include "ext4.h"
#include "ntfs.h"

#define SIZE 102400

int main(int argc, char* argv[])
{
    int fdes;
    int j = 0;
    char buffer[SIZE];
    size_t r;
    
    //opening file from the user
    fdes = open(argv[1], O_RDONLY);

    // reading the contents
    r = read(fdes, buffer, SIZE);

    MASTER_BOOT_RECORD newRecord;
    
    //Take in all the data regarding the Master Boot Record and put it into the struct for later use
    for(int i = 0x0000; i < 0x0200; i++)
    {
        if(i < 0x01B8)
            newRecord.BootStrapCode[i] = buffer[i];

        if(i >= 0x01B8 && i <0x01BC)
            newRecord.UniqueMbrSignature[i - 0x01B8] = buffer[i];
        
        if(i >= 0x01BC && i < 0x01BE)
            newRecord.Unknown[i - 0x01BC] = buffer[i];
        
        if(i >= 0x01BE && i < 0x01FE)
        {
            newRecord.Partition[j].BootIndicator = buffer[i];
            newRecord.Partition[j].StartHead = buffer[i += 1];
            newRecord.Partition[j].StartSector = buffer[i += 1];
            newRecord.Partition[j].StartTrack = buffer[i += 1];
            newRecord.Partition[j].OSIndicator = buffer[i += 1];
            newRecord.Partition[j].EndHead = buffer[i += 1];
            newRecord.Partition[j].EndSector = buffer[i += 1];
            newRecord.Partition[j].EndTrack = buffer[i += 1];
            for(int x = 0; x < 4; x++)
                newRecord.Partition[j].StartingLBA[x] = buffer[i += x];
            for(int x = 0; x < 4; x++)
                newRecord.Partition[j].SizeInLBA[x] = buffer[i += x];
            j++;
        }
    }
    newRecord.Signature = MBR_SIGNATURE;


    if(buffer[0x01FE] == 0 && buffer[0x01FF] == 0)
    {
        struct ext4_super_block newExt;

        newExt.s_inodes_count = (buffer[0x2 + 1024] << 16) | (buffer[0x1 + 1024] << 8) | (buffer[0x0 + 1024] & 0xff);;
        newExt.s_blocks_count_lo = (buffer[0x6 + 1024] << 16) | ((buffer[0x5 + 1024] & 0xff) << 8) | buffer[0x4 + 1024];
        newExt.s_magic = ((buffer[0x39 + 1024] & 0xff) << 8)| buffer[0x38 + 1024];

        for(int i = 0x88; i < 0xC8; i++)
            newExt.s_last_mounted[i - 0x88] = buffer[i + 1024];

        for(int i = 0x78; i < 0x88; i++)
            newExt.s_volume_name[i - 0x78] = buffer[i + 1024];

        for(int i = 0x68; i < 0x78; i++)
            newExt.s_uuid[i - 0x68] = buffer[i + 1024];

        printf("Inode count: %x\n", newExt.s_inodes_count);
        printf("Block count: %x\n", newExt.s_blocks_count_lo);
        printf("Magic number: %x\n", newExt.s_magic);

        printf("Last mounted: ");
        for(int i = 0; i < 64; i++)
            putchar(newExt.s_last_mounted[i]);

        printf("\n");
        printf("Volume Label: ");
        for(int i = 0; i < 16; i++)
            putchar(newExt.s_volume_name[i]);
        printf("\n");

        printf("Volume ID: ");
        for(int i = 0; i < 16; i++)
            printf("%hhx", newExt.s_uuid[i]);
        printf("\n");
    }
    else if(newRecord.Partition[0].OSIndicator == 0x72)
    {
        struct ntfs_layout newNTFS;

        newNTFS.sector_size = (buffer[0x0C] << 8)|buffer[0x0B];
        newNTFS.sectors_per_track = (buffer[0x19] << 8)|buffer[0x18];
        newNTFS.number_of_heads = ((buffer[0x1B] & 0xff) << 8)|buffer[0x1A];
        newNTFS.os_number = ((buffer[0x26] & 0xff) << 16)|(buffer[0x25] << 8)|(buffer[0x24] & 0xff);
        newNTFS.total_sectors = (buffer[0x29] << 8)|(buffer[0x28] & 0xff);

        for(int i = 0x30; i < 0x38; i++)
            newNTFS.starting_cluster[i - 0x30] = buffer[i];

        printf("Sector size: %x\n", newNTFS.sector_size);
        printf("Sector per track: %x\n", newNTFS.sectors_per_track);
        printf("Number of heads: %x\n", newNTFS.number_of_heads);
        printf("OS: %x\n", newNTFS.os_number);
        printf("Total sectors: %x\n", newNTFS.total_sectors);
        printf("Starting cluster: ");
        for(int i = 0; i < 8; i++)
            printf("%hhx", newNTFS.starting_cluster);
        printf("\n");

        printf("Unique MBR signature: ");
        for(int i = 0; i < 4; i++)
            printf("%x", newRecord.UniqueMbrSignature[i]);
        printf("\n");  
    }
    else
        fprintf(stderr, "Unknown File Sytem\n");    

    close(fdes);
}