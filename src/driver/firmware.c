#include <driver/firmware.h>
#include <driver/mailbox.h>

uint8_t get_firmware(Firmware *fw) {
    volatile uint32_t __attribute__((aligned(16))) fwmbox[7];
    fwmbox[0] = 4*7;
    fwmbox[1] = RPI_FIRMWARE_STATUS_REQUEST;
    
    fwmbox[2] = RPI_FIRMWARE_GET_FIRMWARE_REVISION;
    fwmbox[3] = 4;
    fwmbox[4] = 0;
    fwmbox[5] = 0;

    fwmbox[6] = RPI_FIRMWARE_PROPERTY_END;

    if (mbox_call(MAIL_TAGS, (uint64_t)&fwmbox)) {
        fw->revision = fwmbox[5];
        return 1;
    } else {
        return 0;
    }
}