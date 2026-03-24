#ifndef FS_H
#define FS_H

#include <fs/mbr.h>
#include <fs/fat32.h>
#include <driver/emmc.h>
#include <mem.h>
#include <string.h>

#include <driver/lfb.h>

typedef enum FileType {
    FILE_TYPE_DIR,
    FILE_TYPE_BIN,
    FILE_TYPE_TXT
} FileType;

typedef enum FilePermission {
    FILE_READ = (uint8_t)1,
    FILE_WRITE = (uint8_t)2,
    FILE_EXECUTE = (uint8_t)4
} FilePermission;

typedef enum FsGetInfo {
    MBR_GET_STARTING_LBA,
    FS_GET_BYTES_PER_SECTOR,
    FS_GET_SECTORS_PER_CLUSTER,
    FS_GET_RESERVED_SECTORS,
    FS_GET_FAT_SIZE,
    FS_GET_FAT_COUNT,
    FS_GET_ROOT_CLUSTER,
    FS_GET_SECTORS_PER_FAT
} FsGetInfo;

typedef enum FsStatus {
    FS_SUCCESS,
    FS_FAT_CORRUPTION,
    FS_READ_FAIL,
    FS_WRITE_FAIL,
    FS_NO_FS_SUPPORT,
    FS_PART_MOUNTED,
    FS_PART_NOT_MOUNTED,
    FS_FILE_NOT_FOUND,
    FS_FILE_ALREADY_EXISTS,
    FS_INVALID_PATH
} FsStatus;

typedef struct {
    uint32_t id;
    uint8_t type;
    uint8_t permission;
    uint32_t size;
    ustring name[13];
} File;

typedef struct {
    MasterBootRecord *Mbr;
    FAT32BootSector *FAT32Bs[4];
    uint32_t *FAT[4];
    uint32_t *FAT2[4];
    DirEntry *RootDir[4];
    DirEntry *cwd[4];
} FileSystem;

void fs_init(MemoryMap *Mmap, FileSystem *Fs);
uint64_t fs_get_info(FsGetInfo info_request, FileSystem *Fs, uint8_t part);

FsStatus fs_mount(uint8_t part, MemoryMap *Mmap, FileSystem *Fs);
FsStatus fs_unmount(uint8_t part, MemoryMap *Mmap, FileSystem *Fs);
void *fs_load_file(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs, Framebuffer *Fb);
FsStatus fs_rename_partition(const string label, uint8_t part, FileSystem *Fs);

#endif