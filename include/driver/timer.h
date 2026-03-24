#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define MICROS  1
#define MILLIES 1000
#define SECONDS 1000000

typedef struct {
    volatile uint32_t CS;
    volatile uint32_t CLO;
    volatile uint32_t CHI;
    volatile uint32_t C0;
    volatile uint32_t C1;
    volatile uint32_t C2;
    volatile uint32_t C3;
} SystemTimer;

typedef struct {
    volatile uint32_t LOAD;
    volatile uint32_t VALUE;
    volatile uint32_t CONTROL;
    volatile uint32_t IRQCA;
    volatile uint32_t RIRQ;
    volatile uint32_t MIRQ;
    volatile uint32_t RELOAD;
    volatile uint32_t PREDIV;
    volatile uint32_t FREECNT;
} ArmTimer;

uint64_t free_timer();
void delay(uint64_t micros);
void wait_cycles(uint64_t n);

#endif