#ifndef NTFS_H
#define NTFS_H

#include <linux/types.h>

struct ntfs_layout
{
    __u8 dontcare[11];
    __le16 sector_size; // 0x0B-0x0C
    __u8 sectors_per_cluster; //0x0D
    __u8 reserved0e; // 0x0E
    __u8 reserved0f;// 0x0f
    __u8 zero10;//0x10
    __u16 zero11;//0x11-0x12
    __u16 zero13;// 0x13-0x14
    __u8 media_descriptor; // 0x15
    __u16 zero16; //0x16-0x17h
    __le16 sectors_per_track; // 0x18-0x19h
    __le16 number_of_heads; // 0x1A-0x1Bh
    __le32 number_of_sectors_hidden; // 0x1C-0x1Fh
    __u32 zero20; // 0x20-0x23h
    __u32 os_number; //0x24-0x27h
    __le16 total_sectors; //0x28-0x29h
    __le16 starting_cluster[8]; // 0x30-0x37h
    __le16 starting_cluster_number[4]; //0x38-0x39h
    __s8 clusters_per_segment; // 0x40h
    __u8 zero41[3]; // 0x41-0x43h
    __s8 clusters_per_index_buffer; //0x44h
    __u8 zero45[3]; //0x45-0x47h
    __le16 volume_serial_number[4]; //0x48-50h
};

#endif