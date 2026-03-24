#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdint.h>

typedef struct {
    uint32_t revision;
} Firmware;

uint8_t get_firmware(Firmware *fw);

#endif