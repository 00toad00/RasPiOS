#ifndef LFB_H
#define LFB_H

#include <stdint.h>
#include <font.h>
#include <driver/mailbox.h>

typedef struct {
    volatile uint32_t width;
    volatile uint32_t height;
    volatile uint32_t pitch;
    volatile uint32_t pxorder;
    volatile uint32_t backcolor;
    volatile uint32_t forecolor;
    volatile uint32_t sx;
    volatile uint32_t xoff;
    volatile uint32_t sy;
    volatile uint32_t yoff;
    volatile uint32_t ex;
    volatile uint8_t *lfb;
    volatile AsciiFont *Font;
} Framebuffer;

uint8_t lfb_init(Framebuffer *fb);

#endif