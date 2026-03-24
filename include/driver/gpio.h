#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
    volatile uint32_t FSEL0;
    volatile uint32_t FSEL1;
    volatile uint32_t FSEL2;
    volatile uint32_t FSEL3;
    volatile uint32_t FSEL4;
    volatile uint32_t FSEL5;
    volatile uint32_t Reserved0;
    volatile uint32_t SET0;
    volatile uint32_t SET1;
    volatile uint32_t Reserved1;
    volatile uint32_t CLR0;
    volatile uint32_t CLR1;
    volatile uint32_t Reserved2;
    volatile uint32_t LEV0;
    volatile uint32_t LEV1;
    volatile uint32_t Reserved3;
    volatile uint32_t EDS0;
    volatile uint32_t EDS1;
    volatile uint32_t Reserved4;
    volatile uint32_t REN0;
    volatile uint32_t REN1;
    volatile uint32_t Reserved5;
    volatile uint32_t FEN0;
    volatile uint32_t FEN1;
    volatile uint32_t Reserved6;
    volatile uint32_t HEN0;
    volatile uint32_t HEN1;
    volatile uint32_t Reserved7;
    volatile uint32_t LEN0;
    volatile uint32_t LEN1;
    volatile uint32_t Reserved8;
    volatile uint32_t AREN0;
    volatile uint32_t AREN1;
    volatile uint32_t Reserved9;
    volatile uint32_t AFEN0;
    volatile uint32_t AFEN1;
    volatile uint32_t Reserved10;
    volatile uint32_t PUD;
    volatile uint32_t PUDCLK0;
    volatile uint32_t PUDCLK1;
} GPIO;

typedef enum {
    GPF_INPUT = 0b000,
    GPF_OUTPUT = 0b001,
    GPF_ALT0 = 0b100,
    GPF_ALT1 = 0b101,
    GPF_ALT2 = 0b110,
    GPF_ALT3 = 0b111,
    GPF_ALT4 = 0b011,
    GPF_ALT5 = 0b010
} GPIOMode;

typedef enum {
    GPIO_LOW,
    GPIO_HIGH
} GPIOStatus;

void gpio_set_mode(uint8_t pin, GPIOMode mode);
void gpio_set_status(uint8_t pin, GPIOStatus status);
uint8_t gpio_get_status(uint8_t pin);

#endif