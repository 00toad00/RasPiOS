#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

typedef struct {
    uint8_t jump[3];             // 0x00: Jump instruction to boot code
    uint8_t  oem_name[8];         // 0x03: OEM Name (ASCII), e.g., "MSDOS5.0"

    uint16_t bytes_per_sector;    // 0x0B: Bytes per sector (usually 512)
    uint8_t  sectors_per_cluster; // 0x0D: Sectors per cluster
    uint16_t reserved_sectors;    // 0x0E: Number of reserved sectors before FAT
    uint8_t  fat_count;           // 0x10: Number of FATs (usually 2)
    uint16_t root_entry_count;    // 0x11: Max root dir entries (FAT12/16; 0 for FAT32)
    uint16_t total_sectors_16;    // 0x13: Total sectors (if less than 65536)
    uint8_t  media_descriptor;    // 0x15: Media descriptor byte
    uint16_t sectors_per_fat_16;  // 0x16: FAT size (FAT12/16; 0 for FAT32)
    uint16_t sectors_per_track;   // 0x18: CHS info, optional
    uint16_t head_count;          // 0x1A: CHS info, optional
    uint32_t hidden_sectors;      // 0x1C: Sectors before this partition
    uint32_t total_sectors_32;    // 0x20: Total sectors (if >= 65536)

    // FAT32-specific
    uint32_t sectors_per_fat_32;  // 0x24: Sectors per FAT
    uint16_t fat_flags;           // 0x28: Flags (mirroring, etc.)
    uint16_t fs_version;          // 0x2A: Filesystem version (usually 0)
    uint32_t root_cluster;        // 0x2C: First cluster of root directory (usually 2)
    uint16_t fs_info;             // 0x30: FS info sector (usually 1)
    uint16_t backup_boot_sector;  // 0x32: Backup boot sector (usually 6)
    uint8_t  reserved[12];        // 0x34: Reserved / padding

    uint8_t  drive_number;        // 0x40: Drive number (BIOS)
    uint8_t  reserved1;           // 0x41: Reserved
    uint8_t  boot_signature;      // 0x42: Extended boot signature (0x29)
    uint32_t volume_id;           // 0x43: Volume serial number
    uint8_t  volume_label[11];    // 0x47: Volume label (ASCII)
    uint8_t  fs_type[8];          // 0x52: Filesystem type (ASCII "FAT32   ")

    uint8_t  boot_code[420];      // 0x5A: Boot code (rest of boot sector)
    uint16_t signature;           // 0x1FE: Boot sector signature (0x55AA)
} __attribute__((packed)) FAT32BootSector;

#define FAT32_MASK                  0x0FFFFFFF

#define FAT_CLUSTER_UNUSED(x)       (((uint32_t)x & FAT32_MASK) == 0)
#define FAT_CLUSTER_BAD(x)          (((uint32_t)x & FAT32_MASK) == 0x0FFFFFF7)
#define FAT_END_OF_CHAIN(x)         (((uint32_t)x & FAT32_MASK) >= 0x0FFFFFF8)

#define FAT_ENTRY_ATTR_END_ENTRY    0x00
#define FAT_ENTRY_ATTR_VOLUME_LABEL 0x08
#define FAT_ENTRY_ATTR_DIRECTORY    0x10
#define FAT_ENTRY_ATTR_ARCHIVE      0x20
#define FAT_ENTRY_ATTR_LFN          0x0F
#define FAT_ENTRY_ATTR_DELETED      0xE5

typedef uint32_t FAT[];

typedef struct {
    uint8_t  name[11];          // 8.3 name
    uint8_t  attr;
    uint8_t  nt_reserved;
    uint8_t  creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) DirEntry;

/*
[ Boot / Reserved Region ]
    - Boot sector (BPB)
    - FSInfo
    - Backup boot sector (optional)

[ FAT Tables ]
    - FAT #1 (cluster linking table)
    - FAT #2 (backup copy)

[ Data Region ]
    - Root directory cluster(s)
        - Contains 32-byte DirEntries
        - Each entry points to:
            - A subdirectory (cluster chain of DirEntries)
            - Or a file (cluster chain of raw data)
    - Subdirectory clusters
        - Contains 32-byte DirEntries
        - Each entry points further down (subdirs or files)
    - File clusters
        - Raw file data
        - Follow cluster chain via FAT until EOC
*/

#endif