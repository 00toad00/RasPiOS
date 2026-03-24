#ifndef MBR_H
#define MBR_H

#include <stdint.h>

#define MBR_PENTRY_ACTIVE       0x80
#define MBR_PENTRY_INACTIVE     0x00

#define MBR_PARTITION_TYPE_EMPTY        0x00
#define MBR_PARTITION_TYPE_FAT32_LBA    0x0C
#define MBR_PARTITION_TYPE_FAT32        0x0B
#define MBR_PARTITION_TYPE_EXT4         0x83
#define MBR_PARTITION_TYPE_NTFS_EXFAT   0x07

typedef struct {
    uint8_t boot_flag;
    uint8_t starting_chs[3];
    uint8_t partition_type;
    uint8_t ending_chs[3];
    uint32_t starting_lba;
    uint32_t sizes;
} __attribute__((packed)) MBRPartitionEntry;

typedef struct {
    uint8_t boot_code[446];
    MBRPartitionEntry entry[4];
    uint16_t signature;
} __attribute__((packed)) MasterBootRecord;

#endif