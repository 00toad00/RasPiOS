#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define BOARD_MANUF_SONYUK  0
#define BOARD_MANUF_EGOMAN  1
#define BOARD_MANUF_EMBEST  2
#define BOARD_MANUF_SONYJP  3
#define BOARD_MANUF_EMBEST2 4
#define BOARD_MANUF_STADIUM 5

#define BOARD_SOC_BCM2835   0
#define BOARD_SOC_BCM2836   1
#define BOARD_SOC_BCM2837   2
#define BOARD_SOC_BMC2711   3

#define BOARD_RAMS_256MB    0
#define BOARD_RAMS_512MB    1
#define BOARD_RAMS_1GB      2
#define BOARD_RAMS_2GB      3
#define BOARD_RAMS_4GB      4
#define BOARD_RAMS_8GB      5

#define BOARD_TYPE_A        0x00
#define BOARD_TYPE_B        0x01
#define BOARD_TYPE_Ap       0x02
#define BOARD_TYPE_Bp       0x03
#define BOARD_TYPE_2B       0x04
#define BOARD_TYPE_CM1      0x06
#define BOARD_TYPE_3B       0x08
#define BOARD_TYPE_ZERO     0x09
#define BOARD_TYPE_CM3      0x0A
#define BOARD_TYPE_ZEROW    0x0C
#define BOARD_TYPE_3Bp      0x0D
#define BOARD_TYPE_4B       0x11
#define BOARD_TYPE_ZERO2W   0x12
#define BOARD_TYPE_400      0x13
#define BOARD_TYPE_CM4      0x14

typedef struct {
    volatile uint64_t serial;
    volatile uint32_t revision;
    volatile uint8_t rev;
    volatile uint8_t type;
    volatile uint8_t soc;
    volatile uint8_t manuf;
    volatile uint8_t mem;
} Board;

uint8_t get_board_info(Board *bd);

#endif