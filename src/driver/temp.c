#include <driver/temp.h>
#include <driver/mailbox.h>

uint32_t get_temperature() {
    volatile uint32_t __attribute__((aligned(16))) tmbox[8];
    tmbox[0] = 4*8;
    tmbox[1] = RPI_FIRMWARE_STATUS_REQUEST;

    tmbox[2] = RPI_FIRMWARE_GET_TEMPERATURE;
    tmbox[3] = 8;
    tmbox[4] = 0;
    tmbox[5] = 0;
    tmbox[6] = 0;

    tmbox[7] = RPI_FIRMWARE_PROPERTY_END;

    if(mbox_call(MAIL_TAGS, (uint64_t)&tmbox)) {
        return tmbox[6];
    } else {
        return 0;
    }
}

uint32_t get_max_temperature() {
    volatile uint32_t __attribute__((aligned(16))) tmbox[8];
    tmbox[0] = 4*8;
    tmbox[1] = RPI_FIRMWARE_STATUS_REQUEST;

    tmbox[2] = RPI_FIRMWARE_GET_MAX_TEMPERATURE;
    tmbox[3] = 8;
    tmbox[4] = 0;
    tmbox[5] = 0;
    tmbox[6] = 0;

    tmbox[7] = RPI_FIRMWARE_PROPERTY_END;

    if(mbox_call(MAIL_TAGS, (uint64_t)&tmbox)) {
        return tmbox[6];
    } else {
        return 0;
    }
}