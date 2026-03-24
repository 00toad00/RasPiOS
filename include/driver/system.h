#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <peripherals.h>
#include <driver/gpio.h>
#include <driver/spi.h>
#include <driver/mailbox.h>
#include <driver/lfb.h>
#include <driver/timer.h>
#include <driver/board.h>
#include <driver/firmware.h>
#include <fs/fs.h>
#include <mem.h>

typedef struct {
    uint64_t mmiobase;
    uint64_t kernel_start;
    uint64_t kernel_end;
    uint64_t boottime;
    GPIO *Gpio;
    Mailbox *Mbox;
    SPI0 *Spi0;
    SystemTimer *SystemTimer;
    ArmTimer *ArmTimer;
    Framebuffer *Framebuffer;
    Board *Board;
    Firmware *Firmware;
    FileSystem *FileSystem;
} System;

#endif