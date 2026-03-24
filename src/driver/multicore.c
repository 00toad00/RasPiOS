#include <driver/multicore.h>

void slavecore_cmd(uint8_t coreid) {
    asm volatile("sev");
    return;
}