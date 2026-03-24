#include <stdint.h>
#include <peripherals.h>
#include <driver/mailbox.h>
#include <driver/gpio.h>
#include <driver/spi.h>
#include <driver/lfb.h>
#include <driver/system.h>
#include <driver/board.h>
#include <driver/timer.h>
#include <driver/temp.h>
#include <driver/firmware.h>
#include <driver/emmc.h>
#include <driver/multicore.h>
#include <mem.h>
#include <string.h>
#include <video.h>
#include <font.h>
#include <fs/mbr.h>
#include <fs/fat32.h>
#include <fs/fs.h> 

extern Shared cpu_shared_mem;
static Shared *Share __attribute__((aligned(16))) = &cpu_shared_mem;
volatile CPURelease *CpuR = (volatile CPURelease*)CPUR_BASE;

void debug() {
    GPIO *gpio = (GPIO*)GPIO_BASE;
    gpio->FSEL2 &= ~(0b111 << 18);
    gpio->FSEL2 |= (1 << 18);
    while (1) {
        gpio->SET0 = (1 << 26);
        wait_cycles(1000000);
        gpio->CLR0 = (1 << 26);
        wait_cycles(1000000);
    }
}

void kernel_ui(System *Sys) {
    static Framebuffer pfb;
    memcpy(&pfb, Sys->Framebuffer, sizeof(Framebuffer));
    pfb.sx = Sys->Framebuffer->width/4;
    pfb.sy = 20;
    pfb.xoff = 0;
    pfb.yoff = 0;
    pfb.ex = 1900;

    uint32_t Ax = 4;                            uint32_t Ay = 4;
    uint32_t Bx = Sys->Framebuffer->width-4;    uint32_t By = 4;
    uint32_t Cx = 4;                            uint32_t Cy = Sys->Framebuffer->height-5;
    uint32_t Dx = Sys->Framebuffer->width-4;    uint32_t Dy = Sys->Framebuffer->height-5;

    draw_line(0x00FF0000, Ax, Ay, Bx, By, Sys->Framebuffer);
    draw_line(0x00FF0000, Cx, Cy, Dx, Dy, Sys->Framebuffer);
    draw_line(0x00FF0000, Ax, Ay, Cx, Cy, Sys->Framebuffer);
    draw_line(0x00FF0000, Bx, By, Dx, Dy, Sys->Framebuffer);

    uint32_t Ex = (Sys->Framebuffer->width/2)-1; uint32_t Ey = 4;
    uint32_t Fx = (Sys->Framebuffer->width/2)-1; uint32_t Fy = Sys->Framebuffer->height-5;

    draw_line(0x00FF0000, Ex, Ey, Fx, Fy, Sys->Framebuffer);
    draw_line(0x00FF0000, Ex+1, Ey, Fx+1, Fy, Sys->Framebuffer);

    ustring type[11];
    switch (Sys->Board->type) {
        case BOARD_TYPE_3B:
            strcpy(type, "Pi 3B");
            break;
        case BOARD_TYPE_ZERO2W:
            strcpy(type, "Pi Zero 2W");
            break;
        default:
            itoa_hex(Sys->Board->type, type);
    }

    ustring soc[9];
    switch (Sys->Board->soc) {
    case BOARD_SOC_BCM2835:
        strcpy(soc, "BCM 2835");
        break;
    case BOARD_SOC_BCM2836:
        strcpy(soc, "BCM 2836");
        break;
    case BOARD_SOC_BCM2837:
        strcpy(soc, "BCM 2837");
        break;
    case BOARD_SOC_BMC2711:
        strcpy(soc, "BCM 2711");
        break;
    default:
        itoa_hex(Sys->Board->soc, soc);
        break;
    }

    ustring rams[6];
    switch (Sys->Board->mem) {
    case BOARD_RAMS_256MB:
        strcpy(rams, "256MB");
        break;
    case BOARD_RAMS_512MB:
        strcpy(rams, "512MB");
        break;
    case BOARD_RAMS_1GB:
        strcpy(rams, "1GB");
        break;
    case BOARD_RAMS_2GB:
        strcpy(rams, "2GB");
        break;
    case BOARD_RAMS_4GB:
        strcpy(rams, "4GB");
        break;
    case BOARD_RAMS_8GB:
        strcpy(rams, "8GB");
        break;
    default:
        itoa_hex(Sys->Board->mem, rams);
        break;
    }

    ustring manuf[8];
    switch (Sys->Board->manuf) {
        case BOARD_MANUF_SONYUK:
            strcpy(manuf, "Sony UK");
            break;
        case BOARD_MANUF_EGOMAN:
            strcpy(manuf, "Egoman");
            break;
        case BOARD_MANUF_EMBEST:
            strcpy(manuf, "Embest");
            break;
        case BOARD_MANUF_EMBEST2:
            strcpy(manuf, "Embest");
            break;
        case BOARD_MANUF_SONYJP:
            strcpy(manuf, "Sony JP");
            break;
        case BOARD_MANUF_STADIUM:
            strcpy(manuf, "Stadium");
            break;
        default:
            itoa_hex(Sys->Board->manuf, manuf);
            break;
    }

    Sys->Framebuffer->forecolor = 0x00FF0000;
    println("Board\n", Sys->Framebuffer); 
    Sys->Framebuffer->forecolor = 0x00FFFFFF;
    printfi("Serial       : /i\n", Sys->Board->serial, Sys->Framebuffer);
    printfs("Type         : /s\n", type, Sys->Framebuffer);
    printfs("SoC          : /s\n", soc, Sys->Framebuffer);
    printfs("RAM          : /s\n", rams, Sys->Framebuffer);
    printfs("Manufacturer : /s\n", manuf, Sys->Framebuffer);
    nextrow(Sys->Framebuffer);

    Sys->Framebuffer->forecolor = 0x00FF0000;
    println("Firmware\n", Sys->Framebuffer);
    Sys->Framebuffer->forecolor = 0x00FFFFFF;
    printfi("Revision     : /i\n", Sys->Firmware->revision, Sys->Framebuffer);
    nextrow(Sys->Framebuffer);

    Sys->Framebuffer->forecolor = 0x00FF0000;
    println("System\n", Sys->Framebuffer);
    Sys->Framebuffer->forecolor = 0x00FFFFFF;
    println("CORE 0       : Off\n", Sys->Framebuffer);
    println("CORE 1       : Off\n", Sys->Framebuffer);
    println("CORE 2       : Off\n", Sys->Framebuffer);
    println("CORE 3       : Off\n", Sys->Framebuffer);
    printfi("MMIO Base    : /h\n", Sys->mmiobase, Sys->Framebuffer);
    println("Uptime       : \n", Sys->Framebuffer);
    printfi("Max Temp     : /i dC\n", get_max_temperature()/1000, Sys->Framebuffer);
    println("Temp         : \n", Sys->Framebuffer);
    printfi("Kernel Start : /h\n", Sys->kernel_start, Sys->Framebuffer);
    printfi("Kernel End   : /h\n", Sys->kernel_end, Sys->Framebuffer);
    printfi("Kernel Size  : /i B\n", Sys->kernel_end-Sys->kernel_start, Sys->Framebuffer);
    printfi("Boot Time    : /i us\n", Sys->boottime, Sys->Framebuffer);
    nextrow(Sys->Framebuffer);

    Sys->Framebuffer->forecolor = 0x00FF0000;
    println("Video\n", Sys->Framebuffer);
    Sys->Framebuffer->forecolor = 0x00FFFFFF;
    printfi("Framebuffer  : /h\n", (uint64_t)Sys->Framebuffer->lfb, Sys->Framebuffer);
    printfi("Width        : /i\n", Sys->Framebuffer->width, Sys->Framebuffer);
    printfi("Height       : /i\n", Sys->Framebuffer->height, Sys->Framebuffer);
    if (Sys->Framebuffer->pxorder == ARGB) { printfs("Pixel Order  : /s\n", "ARGB", Sys->Framebuffer); }
    else { printfs("Pixel Order  : /s\n", "ABGR", Sys->Framebuffer); }
    nextrow(Sys->Framebuffer);

    ustring bootflag[6];
    ustring ptype[15];
    uint32_t slba;
    uint32_t sizes;
    ustring label[12];
    memset(&label, 0, sizeof(label));
    for (uint8_t i = 0; i < 4; i++) {
        itoa_hex((uint64_t)Sys->FileSystem->Mbr->entry[i].boot_flag, bootflag);
        memcpy(&slba, &Sys->FileSystem->Mbr->entry[i].starting_lba, sizeof(uint32_t));
        memcpy(&sizes, &Sys->FileSystem->Mbr->entry[i].sizes, sizeof(uint32_t));
        switch (Sys->FileSystem->Mbr->entry[i].partition_type) {
            case MBR_PARTITION_TYPE_EMPTY:
                strcpy(ptype, "Empty");
                break;
            case MBR_PARTITION_TYPE_FAT32:
                strcpy(ptype, "FAT32");
                break;
            case MBR_PARTITION_TYPE_FAT32_LBA:
                strcpy(ptype, "FAT32 (LBA)");
                break;
            case MBR_PARTITION_TYPE_EXT4:
                strcpy(ptype, "ext4");
                break;
            case MBR_PARTITION_TYPE_NTFS_EXFAT:
                strcpy(ptype, "NTFS / exFAT");
                break;
            default:
                strcpy(ptype, "Unknown");
                break;
        }

        pfb.forecolor = 0x00FF0000;
        if (i != 0) { nextrow(&pfb); }
        printfi("Partition Entry /i\n", (uint64_t)i, &pfb);
        pfb.forecolor = 0x00FFFFFF;
        printfi("Bootflag : /i\n", Sys->FileSystem->Mbr->entry[i].boot_flag, &pfb);
        printfs("Type     : /s\n", ptype, &pfb);
        if (Sys->FileSystem->Mbr->entry[i].partition_type == MBR_PARTITION_TYPE_FAT32_LBA) {
            membcpy(label, (void*)Sys->FileSystem->FAT32Bs[i]->volume_label, 11);
            printfs("Label    : /s\n", label, &pfb);
            printfi("Start    : Sector /i\n", slba, &pfb);
            printfi("Size     : /i Sectors\n", sizes, &pfb);
        }
    }

    uint64_t timer0 = 0;
    uint64_t timer1 = 0;
    uint64_t timer2 = 0;
    ustring time[10];
    ustring temp[10];
    uint32_t xoff = 0;
    while (1) {
        uint64_t now = free_timer();
        if ((now/1000)-timer0 >= 1000) {
            xoff = printsp(time, Sys->Framebuffer->backcolor, 
                Sys->Framebuffer->sx+10*15, Sys->Framebuffer->sy+15*16, Sys->Framebuffer);
            printsp(" s", Sys->Framebuffer->backcolor, xoff, Sys->Framebuffer->sy+15*16, Sys->Framebuffer);
            itoa_int(now/1000000, time);
            xoff = printsp(time, Sys->Framebuffer->forecolor, 
                Sys->Framebuffer->sx+10*15, Sys->Framebuffer->sy+15*16, Sys->Framebuffer);
            printsp(" s", Sys->Framebuffer->forecolor, xoff, Sys->Framebuffer->sy+15*16, Sys->Framebuffer);
            timer0 = now/1000;
        }
        if ((now/1000)-timer1 >= 250) {
            xoff = printsp(temp, Sys->Framebuffer->backcolor,
                Sys->Framebuffer->sx+10*15, Sys->Framebuffer->sy+15*18, Sys->Framebuffer);
            printsp(" dC", Sys->Framebuffer->backcolor, xoff, Sys->Framebuffer->sy+15*18, Sys->Framebuffer);
            itoa_int(get_temperature()/1000, temp);
            xoff = printsp(temp, Sys->Framebuffer->forecolor,
                Sys->Framebuffer->sx+10*15, Sys->Framebuffer->sy+15*18, Sys->Framebuffer);
            printsp(" dC", Sys->Framebuffer->forecolor, xoff, Sys->Framebuffer->sy+15*18, Sys->Framebuffer);
            timer1 = now/1000;
        }
        if ((now/1000)-timer2 >= 250) {
            for (uint8_t i = 0; i < 4; i++) {
                if (Share->Core[i].status & CORE_ON && Share->Core[i].status & CORE_READY) {
                    printsp("Running", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Off", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("On", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Ready", Sys->Framebuffer->forecolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                } else if (Share->Core[i].status & CORE_ON && !(Share->Core[i].status & CORE_READY)) {
                    printsp("Ready", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Off", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("On", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Running", Sys->Framebuffer->forecolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);                    
                } else if (Share->Core[i].status & CORE_OFF) {
                    printsp("Ready", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Running", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("On", Sys->Framebuffer->backcolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);
                    printsp("Off", Sys->Framebuffer->forecolor, Sys->Framebuffer->sx+10*15, 
                        Sys->Framebuffer->sy+15*(11+i), Sys->Framebuffer);                    
                }            
            }
            timer2 = now/1000;
        }
    } 
}

void kernel() {
    memset(Share, 0, 0x400);
    Share->Core[0].status = CORE_ON;
    for (uint8_t i = 1; i < 4; i++) { Share->Core[i].status = CORE_OFF; }

    static System sys;
    static System *Sys = (System*)&sys;
    Share->System = Sys;

    extern void entry1(void);
    CpuR->CPU1 = (uint64_t)entry1;
    extern void entry2(void);
    CpuR->CPU2 = (uint64_t)entry2;
    extern void entry3(void);
    CpuR->CPU3 = (uint64_t)entry3;
    asm volatile("sev");

    static Framebuffer fb;
    static Board bd;
    static Firmware fw;
    static GPIO *gpio = (GPIO*)GPIO_BASE;
    static Mailbox *mbox = (Mailbox*)MBOX_BASE;
    static SPI0 *spi0 = (SPI0*)SPI0_BASE;
    static SystemTimer *st = (SystemTimer*)SYSTIMER_BASE;
    static ArmTimer *at = (ArmTimer*)ARMTIMER_BASE;
    static FileSystem fs;

    lfb_init(&fb);
    get_board_info(&bd);
    get_firmware(&fw);
    sd_init();
    
    extern char kernel_end;
    sys.mmiobase = MMIO_BASE;
    sys.kernel_start = (uint64_t)0x80000; // !!!!!!!!!!!!!!!!!!
    sys.kernel_end = (uint64_t)&kernel_end;
    sys.Framebuffer = &fb;
    sys.Board = &bd;
    sys.Firmware = &fw;
    sys.Gpio = gpio;
    sys.Mbox = mbox;
    sys.Spi0 = spi0;
    sys.SystemTimer = st;
    sys.ArmTimer = at;
    sys.FileSystem = &fs;

    static AsciiFont fnt;
    fnt.height = 13;
    fnt.width = 8;
    fnt.fontmap = (uint8_t*)&ascii_font;
    fb.backcolor = 0x00000000;
    fb.forecolor = 0x00FFFFFF;
    fb.sx = 20;
    fb.sy = 20;
    fb.xoff = 0;
    fb.yoff = 0;
    fb.ex = fb.width/2 - 20;
    fb.Font = &fnt;
    clear_screen(&fb);

    static MemoryMap bsmmap;
    static buffer bsmembuf[2];
    memset(&bsmembuf, 0, sizeof(bsmembuf));
    bsmmap.bufsize = sizeof(bsmembuf);
    bsmmap.cluster_size = SECTOR;
    bsmmap.start = ((uint64_t)&kernel_end + 0x20000 + 7) & ~7;
    bsmmap.bufstart = (uint8_t*)&bsmembuf;
    fs_init(&bsmmap, &fs);

    static MemoryMap fsmmap;
    static buffer fsmembuf[1000];
    memset(&fsmembuf, 0, sizeof(fsmembuf));
    fsmmap.bufsize = sizeof(fsmembuf);
    fsmmap.cluster_size = 4*KiB;
    fsmmap.start = ((uint64_t)&kernel_end + 0x30000 + 7) & ~7;
    fsmmap.bufstart = (uint8_t*)&fsmembuf;

    fs_mount(0, &fsmmap, Sys->FileSystem);
    fs_rename_partition("SYS        ", 0, Sys->FileSystem);

    Framebuffer logfb;
    memcpy(&logfb, Sys->Framebuffer, sizeof(Framebuffer));
    logfb.sx = Sys->Framebuffer->width/2 + 20;
    logfb.sy = 20;
    logfb.ex = Sys->Framebuffer->width/2 - 20;

    //////////////////////////////////////////////////
    uint8_t part = 0;

    ustring filename[12];
    filename[11] = 0;
    for (uint8_t i = 0; Sys->FileSystem->cwd[part][i].name[0] != FAT_ENTRY_ATTR_END_ENTRY; i++) {
        if (Sys->FileSystem->cwd[part][i].name[0] == FAT_ENTRY_ATTR_DELETED) { continue; }
        if (Sys->FileSystem->cwd[part][i].attr == FAT_ENTRY_ATTR_VOLUME_LABEL) { continue; }
        if (Sys->FileSystem->cwd[part][i].attr == FAT_ENTRY_ATTR_LFN) { continue; }
        membcpy(filename, &Sys->FileSystem->cwd[part][i], 11);
        printfs("/s\n", filename, &logfb);
    }
    //////////////////////////////////////////////////

    Sys->boottime = free_timer();
    kernel_ui(Sys);

    Share->Core[0].status = CORE_OFF;
    return;
}

void slavecore(uint64_t coreid) {
    Share->Core[coreid].status = CORE_ON;

    while (1) {
        Share->Core[coreid].status |= CORE_READY;
        asm volatile("wfe");

        if (!(Share->Core[coreid].status & CORE_CMD)) { continue; }
        Share->Core[coreid].status &= ~CORE_READY;
        Share->Core[coreid].status &= ~CORE_SUCCESS;
        Share->Core[coreid].status &= ~CORE_FAIL;

        if (Share->Core[coreid].Cmd.cmd == CORE_CMD_SHUTDOWN) {
            Share->Core[coreid].status |= CORE_SUCCESS;
            break;
        } if (Share->Core[coreid].Cmd.cmd == CORE_CMD_EXEC) {
            Share->Core[coreid].status |= CORE_SUCCESS;
        } else {
            Share->Core[coreid].status |= CORE_FAIL;
        }
        Share->Core[coreid].status &= ~CORE_CMD;
    }

    Share->Core[coreid].status = CORE_OFF;
    
    return;
}