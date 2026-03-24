#include <driver/timer.h>
#include <peripherals.h>

uint64_t free_timer() {
    SystemTimer *st = (SystemTimer*)SYSTIMER_BASE;
    return ((uint64_t)st->CHI << 32) | (uint64_t)st->CLO;
}

void delay(uint64_t micros) {
    uint64_t now = free_timer();
    while (free_timer()-now < micros);
    return;
}

void wait_cycles(uint64_t n) { 
    while (n--) { asm volatile("nop"); }
}