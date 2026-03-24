#ifndef PERIPHERALS_H
#define PERIPHERALS_H

/*
#define BCM2835
#define BCM2837
*/

#define BCM2837

#ifdef BCM2837
#define MMIO_BASE       (uint64_t)0x3F000000
#endif
#ifdef BCM2835     
#define MMIO_BASE       (uint64_t)0x3F000000
#endif

#define SYSTIMER_BASE   (MMIO_BASE + 0x3000)
#define ARMTIMER_BASE   (MMIO_BASE + 0xB400)
#define MBOX_BASE       (MMIO_BASE + 0xB880)
#define GPIO_BASE       (MMIO_BASE + 0x200000)
#define SPI0_BASE       (MMIO_BASE + 0x204000)
#define EMMC_BASE       (MMIO_BASE + 0x300000)

#endif