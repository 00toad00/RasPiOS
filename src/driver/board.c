#include <driver/board.h>
#include <driver/mailbox.h>

uint8_t get_board_info(Board *bd) {
    volatile uint32_t __attribute__((aligned(16))) bimbox[12];
    bimbox[0] = 4*12;
    bimbox[1] = RPI_FIRMWARE_STATUS_REQUEST;

    bimbox[2] = RPI_FIRMWARE_GET_BOARD_SERIAL;
    bimbox[3] = 8;
    bimbox[4] = 0;
    bimbox[5] = 0;
    bimbox[6] = 0;

    bimbox[7] = RPI_FIRMWARE_GET_BOARD_REVISION;
    bimbox[8] = 4;
    bimbox[9] = 0;
    bimbox[10] = 0;

    bimbox[11] = RPI_FIRMWARE_PROPERTY_END;

    if (mbox_call(MAIL_TAGS, (uint64_t)&bimbox)) {
        bd->serial = ((uint64_t)bimbox[6] << 32) | (uint64_t)bimbox[5];
        bd->revision = bimbox[10];
        bd->rev = (bimbox[10] >> 0) & 0xF;
        bd->type = (bimbox[10] >> 4) & 0xFF;
        bd->soc = (bimbox[10] >> 12) & 0xF;
        bd->manuf = (bimbox[10] >> 16) & 0xF;
        bd->mem = (bimbox[10] >> 20) & 0x7;
        return 1;
    } else {
        return 0;
    }
}