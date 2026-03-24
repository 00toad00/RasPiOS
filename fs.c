#include <fs/fs.h>

#include <video.h>

void fs_init(MemoryMap *Mmap, FileSystem *Fs) {
    Fs->Mbr = (MasterBootRecord*)smalloc(Mmap, sizeof(MasterBootRecord));
    sd_readblock(0, Fs->Mbr, 1);
    uint32_t startlba;
    for (uint8_t i = 0; i < 4; i++) {
        if (Fs->Mbr->entry[i].partition_type == MBR_PARTITION_TYPE_FAT32_LBA) {
            Fs->FAT32Bs[i] = (FAT32BootSector*)smalloc(Mmap, sizeof(FAT32BootSector));
            memcpy(&startlba, &Fs->Mbr->entry[i].starting_lba, sizeof(uint32_t));
            sd_readblock(startlba, Fs->FAT32Bs[i], 1);
            Fs->FAT[i] = 0;
            Fs->FAT2[i] = 0;
            Fs->RootDir[i] = 0;
            Fs->cwd[i] = 0;
        }
    }
}

uint64_t fs_get_info(FsGetInfo info_request, FileSystem *Fs, uint8_t part) {
    uint8_t rtn8;
    uint16_t rtn16;
    uint32_t rtn32;
    uint64_t rtn;
    switch (info_request) {
    case MBR_GET_STARTING_LBA:
        membcpy(&rtn32, &Fs->Mbr->entry[part].starting_lba, sizeof(uint32_t));
        rtn = (uint64_t)rtn32;
        break;
    case FS_GET_BYTES_PER_SECTOR:
        membcpy(&rtn16, &Fs->FAT32Bs[part]->bytes_per_sector, sizeof(uint16_t));
        rtn = (uint64_t)rtn16;
        break;
    case FS_GET_SECTORS_PER_CLUSTER:
        membcpy(&rtn8, &Fs->FAT32Bs[part]->sectors_per_cluster, sizeof(uint8_t));
        rtn = (uint64_t)rtn8;
        break;
    case FS_GET_RESERVED_SECTORS:
        membcpy(&rtn16, &Fs->FAT32Bs[part]->reserved_sectors, sizeof(uint16_t));
        rtn = (uint64_t)rtn16;
        break;
    case FS_GET_FAT_COUNT:
        membcpy(&rtn8, &Fs->FAT32Bs[part]->fat_count, sizeof(uint8_t));
        rtn = (uint64_t)rtn8;
        break;
    case FS_GET_ROOT_CLUSTER:
        membcpy(&rtn32, &Fs->FAT32Bs[part]->root_cluster, sizeof(uint32_t));
        rtn = (uint64_t)rtn32;
        break;
    case FS_GET_SECTORS_PER_FAT:
        membcpy(&rtn32, &Fs->FAT32Bs[part]->sectors_per_fat_32, sizeof(uint32_t));
        rtn = (uint64_t)rtn32;
        break;
    default:
        rtn = 0;
        break;
    }
    return rtn;
}

FsStatus fs_mount(uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    if (Fs->Mbr->entry[part].partition_type != MBR_PARTITION_TYPE_FAT32_LBA) { return FS_NO_FS_SUPPORT; }
    if (Fs->FAT[part]) { return FS_PART_MOUNTED; }
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);

    Fs->FAT[part] = (uint32_t*)smalloc(Mmap, fat_size*bps);
    Fs->FAT2[part] = (uint32_t*)smalloc(Mmap, fat_size*bps);
    sd_readblock(starting_lba+reserved_sec, Fs->FAT[part], fat_size);
    sd_readblock(starting_lba+reserved_sec+fat_size, Fs->FAT2[part], fat_size);

    uint32_t cluster = root_cluster;
    uint32_t ncluster = 0;
    for (uint32_t i = 0; i < (fat_size*bps)/4; i++) {
        if (FAT_END_OF_CHAIN(Fs->FAT[part][cluster])) {
            ncluster++;
            break;
        }
        cluster = Fs->FAT[part][cluster];
        ncluster++;
    }
    Fs->RootDir[part] = (DirEntry*)smalloc(Mmap, bps*spc*ncluster);
    cluster = root_cluster;
    ncluster = 0;
    for (uint32_t i = 0; i < (fat_size*bps)/4; i++) {
        if (FAT_END_OF_CHAIN(Fs->FAT[part][cluster])) {
            sd_readblock(data_start + (cluster-2)*spc, (uint8_t*)(Fs->RootDir[part])+ncluster*spc*bps, spc);
            break;
        }
        sd_readblock(data_start + (cluster-2)*spc, (uint8_t*)(Fs->RootDir[part])+ncluster*spc*bps, spc);
        cluster = Fs->FAT[part][cluster];
        ncluster++;
    }
    Fs->cwd[part] = Fs->RootDir[part];
    return FS_SUCCESS;
}

FsStatus fs_unmount(uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    if (Fs->Mbr->entry[part].partition_type != MBR_PARTITION_TYPE_FAT32_LBA) { return FS_NO_FS_SUPPORT; }
    if (!Fs->FAT[part]) { return FS_PART_NOT_MOUNTED; }
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);

    uint32_t cluster = root_cluster;
    uint32_t ncluster = 0;
    for (uint32_t i = 0; i < (fat_size*bps)/4; i++) {
        if (FAT_END_OF_CHAIN(Fs->FAT[part][cluster])) {
            ncluster++;
            break;
        }
        cluster = Fs->FAT[part][cluster];
        ncluster++;
    }
    sfree(Mmap, Fs->FAT[part], fat_size*bps);
    sfree(Mmap, Fs->FAT2[part], fat_size*bps);
    sfree(Mmap, Fs->RootDir[part], bps*spc*ncluster);

    Fs->FAT[part] = 0;
    Fs->FAT2[part] = 0;
    Fs->RootDir[part] = 0;
    Fs->cwd[part] = 0;
    return FS_SUCCESS;
}

/*
void *fs_load_file(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs, Framebuffer *Fb) { 
    if (Fs->Mbr->entry[part].partition_type != MBR_PARTITION_TYPE_FAT32_LBA) { return (void*)FS_NO_FS_SUPPORT; }
    if (Fs->FAT[part] == 0) { return (void*)FS_PART_NOT_MOUNTED; }
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);

    uint32_t starting_cluster = ((uint32_t)Fs->cwd[part]->first_cluster_high << 16)
                                | (uint32_t)Fs->cwd[part]->first_cluster_low;
    uint32_t cluster = starting_cluster;
    ustring filename[13];
    filename[12];

    uint16_t len = 0;
    for (uint16_t i = 0; path[i] != 0; i++) {
        if (path[i] == '/') {
            len = 0;
            continue;
        }
        filename[len] = path[i];
        if (path[i+1] == 0) {}
    }

    return (void*)FS_SUCCESS;
}

FsStatus fs_create_file(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);    
    return FS_SUCCESS;
}

FsStatus fs_create_dir(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);    
    return FS_SUCCESS;
}

FsStatus fs_delete_file(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);    
    return FS_SUCCESS;
}

FsStatus fs_delete_dir(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);    
    return FS_SUCCESS;
}

void *fs_load_file(const string path, uint8_t part, MemoryMap *Mmap, FileSystem *Fs) {
    uint16_t bps = (uint16_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);
    uint32_t start_cluster = 0;
    uint32_t cluster = 0;

    ustring filename[12];
    ustring name[8];
    ustring type[3];
    memset(&filename, 0, 12);
    memset(&name, 0, 8);
    memset(&type, 0, 3);
    membcpy(filename, path, 12);
    strconv(filename, STRCONV_UPPER);
    for (uint8_t i = 0; i < 12; i++) {
        if (filename[i] == '.') {
            membcpy(&name, &filename[0], i);
            membcpy(&type, &filename[i+1], 3);
            break;
        }
    }

    for (uint8_t i = 0; i < 8; i++) {
        if (name[i] == 0) { name[i] = ' '; }
    }
    membcpy(&filename[0], &name, 8);
    for (uint8_t i = 0; i < 3; i++) {
        if (type[i] == 0) { type[i] = ' '; }
    }
    membcpy(&filename[8], &type, 3);
    filename[11] = 0;

    for (uint32_t i = 0; i < (bps*spc)/32; i++) {
        if (memeq(filename, Fs->RootDir[part][i].name, 11)) { 
            start_cluster = ((uint32_t)Fs->RootDir[part][i].first_cluster_high << 16) 
                            | (uint32_t)Fs->RootDir[part][i].first_cluster_low;
            break;
        }
    }
    if (!start_cluster) { return 0; }
    uint32_t ncluster = 0;
    cluster = start_cluster;
    for (uint64_t i = 0; i < (fat_size*bps)/4; i++) {
        if (FAT_END_OF_CHAIN(Fs->FAT[part][cluster])) {
            ncluster++;
            break;
        }
        cluster = Fs->FAT[part][cluster];
        ncluster++;
    }
    void *file = smalloc(Mmap, ncluster*spc*bps + sizeof(File));
    //File *filemeta = (File*)file;
    cluster = start_cluster;
    ncluster = 0;
    for (uint64_t i = 0; i < (fat_size*bps)/4; i++) {
        if (FAT_END_OF_CHAIN(Fs->FAT[part][cluster])) {
            sd_readblock(data_start + (cluster-2)*spc, (uint8_t*)(file+(ncluster*spc*bps) + sizeof(File)), spc);
            ncluster++;
            break;
        }
        cluster = Fs->FAT[part][cluster];
        sd_readblock(data_start + (cluster-2)*spc, (uint8_t*)(file+(ncluster*spc*bps) + sizeof(File)), spc);
        ncluster++;
    }
    return file;
}*/

FsStatus fs_rename_partition(const string label, uint8_t part, FileSystem *Fs) {
    if (Fs->FAT[part] == 0) { return FS_PART_NOT_MOUNTED; }
    ustring slabel[11];
    memset(&slabel, 0, 11);
    membcpy(slabel, label, 11);
    for (uint8_t i = 0; i < 11; i++) {
        if (slabel[i] == 0) { slabel[i] = ' '; }
    }
    membcpy(Fs->FAT32Bs[part]->volume_label, slabel, 11);

    uint32_t bps = (uint32_t)fs_get_info(FS_GET_BYTES_PER_SECTOR, Fs, part);
    uint8_t spc = (uint8_t)fs_get_info(FS_GET_SECTORS_PER_CLUSTER, Fs, part);
    uint32_t starting_lba = (uint32_t)fs_get_info(MBR_GET_STARTING_LBA, Fs, part);
    uint16_t reserved_sec = (uint16_t)fs_get_info(FS_GET_RESERVED_SECTORS, Fs, part);
    uint8_t fat_count = (uint8_t)fs_get_info(FS_GET_FAT_COUNT, Fs, part);
    uint32_t fat_size = (uint32_t)fs_get_info(FS_GET_SECTORS_PER_FAT, Fs, part);
    uint32_t data_start = starting_lba + reserved_sec + (fat_count*fat_size);
    uint32_t root_cluster = (uint32_t)fs_get_info(FS_GET_ROOT_CLUSTER, Fs, part);

    for (uint32_t i = 0; i < (bps*spc)/32; i++) {
        if (Fs->RootDir[part][i].name[0] == 0x00) { break; }
        if (Fs->RootDir[part][i].attr == 0x08) {
            membcpy(Fs->RootDir[part][i].name, slabel, 11);
            sd_writeblock(Fs->RootDir[part], data_start + (root_cluster-2)*spc, spc);
            break;
        }
    }
    return FS_SUCCESS;
}