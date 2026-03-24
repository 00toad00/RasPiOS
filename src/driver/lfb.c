#include <driver/lfb.h>

uint8_t lfb_init(Framebuffer *fb) {
    volatile uint32_t __attribute__((aligned(16))) fbmbox[36];

    fbmbox[0] = 35*4;
    fbmbox[1] = RPI_FIRMWARE_STATUS_REQUEST;

    fbmbox[2] = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
    fbmbox[3] = 8;
    fbmbox[4] = 8;
    fbmbox[5] = 1920;         //FrameBufferInfo.width
    fbmbox[6] = 1080;          //FrameBufferInfo.height

    fbmbox[7] = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
    fbmbox[8] = 8;
    fbmbox[9] = 8;
    fbmbox[10] = 1920;        //FrameBufferInfo.virtual_width
    fbmbox[11] = 1080;         //FrameBufferInfo.virtual_height

    fbmbox[12] = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_OFFSET;
    fbmbox[13] = 8;
    fbmbox[14] = 8;
    fbmbox[15] = 0;           //FrameBufferInfo.x_offset
    fbmbox[16] = 0;           //FrameBufferInfo.y.offset

    fbmbox[17] = RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH;
    fbmbox[18] = 4;
    fbmbox[19] = 4;
    fbmbox[20] = 32;          //FrameBufferInfo.depth

    fbmbox[21] = RPI_FIRMWARE_FRAMEBUFFER_SET_PIXEL_ORDER;
    fbmbox[22] = 4;
    fbmbox[23] = 4;
    fbmbox[24] = ARGB;           //RGB, not BGR preferably

    fbmbox[25] = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
    fbmbox[26] = 8;
    fbmbox[27] = 8;
    fbmbox[28] = 4096;        //FrameBufferInfo.pointer
    fbmbox[29] = 0;           //FrameBufferInfo.size

    fbmbox[30] = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH;
    fbmbox[31] = 4;
    fbmbox[32] = 4;
    fbmbox[33] = 0;           //FrameBufferInfo.pitch

    fbmbox[34] = RPI_FIRMWARE_PROPERTY_END;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MAIL_TAGS, (uint64_t)&fbmbox) && fbmbox[20]==32 && fbmbox[28]!=0) {
        fbmbox[28]&=0x3FFFFFFF;
        fb->lfb=(void*)(bus_arm_addr((uint64_t)fbmbox[28], BUS_TO_ARM));   //convert GPU address to ARM address
        fb->width=fbmbox[5];          //get actual physical width
        fb->height=fbmbox[6];         //get actual physical height
        fb->pitch=fbmbox[33];         //get number of bytes per line
        fb->pxorder=fbmbox[24];         //get the actual channel order
        return 1;
    }
    return 0;
}