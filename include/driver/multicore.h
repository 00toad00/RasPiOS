#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <driver/system.h>

#define CPUR_BASE (uint64_t)0xd8

typedef struct {
    volatile uint64_t CPU0;
    volatile uint64_t CPU1;
    volatile uint64_t CPU2;
    volatile uint64_t CPU3;
} __attribute__((packed)) CPURelease;

typedef enum {
    CORE_ON = 1,
    CORE_OFF = 2,
    CORE_IDLE = 4,
    CORE_READY = 8,
    CORE_SUCCESS = 16,
    CORE_FAIL = 32,
    CORE_CMD = 64
} CoreStatus;

typedef enum {
    CORE_CMD_SHUTDOWN,
    CORE_CMD_EXEC
} CoreCommand;

typedef struct {
    volatile uint32_t cmd;
    volatile uint64_t arg[10];
} CoreCmd;

typedef struct {
    volatile uint32_t status;
    CoreCmd Cmd;
} CoreInterface;

typedef struct {
    CoreInterface Core[4];
    System *System;
} Shared;

#endif