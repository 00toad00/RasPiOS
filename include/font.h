#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct {
    uint8_t height;
    uint8_t width;
    uint8_t *fontmap;
} AsciiFont;

#endif