#include <driver/mailbox.h>
#include <peripherals.h>

uint8_t mbox_call(uint8_t ch, uint64_t mbaddr) {
    Mailbox *mailbox = (Mailbox*)MBOX_BASE;
    //unsigned int r = ((uint32_t)((uint64_t)&mbox)&~0xF) | (ch&0xF);
    unsigned int r = ((uint32_t)((uint64_t)mbaddr)&~0xF) | (ch&0xF);
    /* wait until we can write to the mailbox */
    do {
        asm volatile("nop");
    } while(mailbox->STATUS & RPI_FIRMWARE_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    mailbox->WRITE = r;
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do {
            asm volatile("nop");
        } while(mailbox->STATUS & RPI_FIRMWARE_EMPTY);
        /* is it a response to our message? */
        if(r == mailbox->READ) {
            /* is it a valid successful response? */
            uint32_t *mboxptr = (uint32_t*)mbaddr;
            return mboxptr[1]==RPI_FIRMWARE_STATUS_SUCCESS;
        }
    }
    return 0;
}

uint64_t bus_arm_addr(uint64_t bus, BusArmConv conv) {
    if (conv == BUS_TO_ARM) { return bus & 0x3FFFFFFF; }
    return 0;
}