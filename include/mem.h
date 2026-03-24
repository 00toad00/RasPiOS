#ifndef MEM_H
#define MEM_H

#include <stdint.h>

typedef uint8_t buffer;

#define SECTOR  512
#define KiB     1024
#define MiB     1024*KB
#define GiB     1024*GB

#define KB      1000
#define MB      1000*KB
#define GB      1000*MB   

typedef struct {
    uint8_t *bufstart;
    uint32_t cluster_size;
    uint32_t bufsize;
    uint64_t start;
} MemoryMap;

void *memcpy(void *dest, const void *src, uint64_t size);
void *membcpy(void *dest, const void *src, uint64_t size);
void *memset(void *start, uint64_t value, uint64_t n);
void *membset(void *start, uint8_t value, uint64_t size);
uint8_t memeq(const void *mem1, const void *mem2, uint64_t size);
void *smalloc(MemoryMap *mmap, uint64_t size);
void sfree(MemoryMap *mmap, void *ptr, uint64_t size);

#endif