#include <mem.h>

void *memcpy(void *dest, const void *src, uint64_t size) {
    volatile uint64_t *dest64 = (volatile uint64_t*)dest;
    volatile uint64_t *src64 = (volatile uint64_t*)src;
    volatile uint8_t *dest8 = (volatile uint8_t*)dest;
    volatile uint8_t *src8 = (volatile uint8_t*)src;
    uint32_t words = size / 8;
    uint8_t bytes = size % 8;
    for (uint64_t i = 0; i < words; i++) { dest64[i] = src64[i]; }
    for (uint8_t i = 0; i < bytes; i++) { dest8[words*8+i] = src8[words*8+i]; }
    return dest;
}

void *membcpy(void *dest, const void *src, uint64_t size) {
    volatile uint8_t *dest8 = (volatile uint8_t*)dest;
    volatile uint8_t *src8 = (volatile uint8_t*)src;
    for (uint64_t i = 0; i < size; i++) { dest8[i] = src8[i]; }
    return dest;
}

void *memset(void *start, uint64_t value, uint64_t size) {
    uint64_t *start64 = (uint64_t*)start;
    uint8_t *start8 = (uint8_t*)start;
    uint64_t words = size / 8;
    uint8_t bytes = size % 8;
    for (uint64_t i = 0; i < words; i++) { start64[i] = value; }
    for (uint8_t i = 0; i < bytes; i++) { start8[words*8+i] = (uint8_t)value; }
    return start;
}

void *membset(void *start, uint8_t value, uint64_t size) {
    uint8_t *start8 = (uint8_t*)start;
    for (uint8_t i = 0; i < size; i++) { start8[i] = value; }
    return start;
}

uint8_t memeq(const void* mem1, const void* mem2, uint64_t size) {
    uint64_t *mema64 = (uint64_t*)mem1;
    uint64_t *memb64 = (uint64_t*)mem2;
    uint8_t *mema8 = (uint8_t*)mem1;
    uint8_t *memb8 = (uint8_t*)mem2;
    uint64_t words = size / 8;
    uint64_t bytes = size % 8;
    for (uint64_t i = 0; i < words; i++) {
        if (mema64[i] != memb64[i]) { return 0; }
    }
    for (uint8_t i = 0; i < bytes; i++) {
        if (mema8[words*8+i] != memb8[words*8+i]) { return 0; }
    }
    return 1;
}

void *smalloc(MemoryMap *mmap, uint64_t size) {
    uint64_t ncluster = size / mmap->cluster_size;
    if (size % mmap->cluster_size) { ncluster++; }
    uint64_t consec = 0;
    uint64_t endcluster = 0;
    for (uint64_t i = 0; i < mmap->bufsize; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            if (!(mmap->bufstart[i] & (1 << j))) { 
                consec++;
                endcluster = 8*i + j;
            }
            else { consec = 0; }
            if (consec == ncluster) { goto cfound; }
        }
    }
    return 0;

    cfound:
        uint64_t first = endcluster - ncluster + 1;
        for (uint64_t k = first; k <= endcluster; k++) {
            mmap->bufstart[k / 8] |= (1 << (k % 8));
        }
        return (void*)(mmap->start + first * mmap->cluster_size);
}

void sfree(MemoryMap *mmap, void *ptr, uint64_t size) {
    if (!ptr || size == 0) return;

    uint64_t first = ((uint64_t)ptr - mmap->start) / mmap->cluster_size;
    uint64_t ncluster = (size + mmap->cluster_size - 1) / mmap->cluster_size;
    uint64_t endcluster = first + ncluster - 1;

    // mark clusters as free
    for (uint64_t k = first; k <= endcluster; k++) {
        mmap->bufstart[k / 8] &= ~(1 << (k % 8));
    }
}