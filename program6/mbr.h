/** @file
  Legacy Master Boot Record Format Definition.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MBR_H_
#define _MBR_H_

#define MBR_SIGNATURE               0xaa55

#define EXTENDED_DOS_PARTITION      0x05
#define EXTENDED_WINDOWS_PARTITION  0x0F

#define MAX_MBR_PARTITIONS          4

#define PMBR_GPT_PARTITION          0xEE
#define EFI_PARTITION               0xEF

#define MBR_SIZE                    512

#pragma pack(1)
///
/// MBR Partition Entry
///
typedef struct {
  uint8_t BootIndicator;
  uint8_t StartHead;
  uint8_t StartSector;
  uint8_t StartTrack;
  uint8_t OSIndicator;
  uint8_t EndHead;
  uint8_t EndSector;
  uint8_t EndTrack;
  uint8_t StartingLBA[4];
  uint8_t SizeInLBA[4];
} MBR_PARTITION_RECORD;

///
/// MBR Partition Table
///
typedef struct {
  uint8_t                 BootStrapCode[440];
  uint8_t                 UniqueMbrSignature[4];
  uint8_t                 Unknown[2];
  MBR_PARTITION_RECORD  Partition[MAX_MBR_PARTITIONS];
  uint16_t                Signature;
} MASTER_BOOT_RECORD;

#pragma pack()

#endif