#ifndef EMMC_H
#define EMMC_H

#include <stdint.h>

#define SD_OK                0
#define SD_TIMEOUT          -1
#define SD_ERROR            -2

#define EMMC_ARG2           ((volatile unsigned int*)(MMIO_BASE+0x00300000))
#define EMMC_BLKSIZECNT     ((volatile unsigned int*)(MMIO_BASE+0x00300004))
#define EMMC_ARG1           ((volatile unsigned int*)(MMIO_BASE+0x00300008))
#define EMMC_CMDTM          ((volatile unsigned int*)(MMIO_BASE+0x0030000C))
#define EMMC_RESP0          ((volatile unsigned int*)(MMIO_BASE+0x00300010))
#define EMMC_RESP1          ((volatile unsigned int*)(MMIO_BASE+0x00300014))
#define EMMC_RESP2          ((volatile unsigned int*)(MMIO_BASE+0x00300018))
#define EMMC_RESP3          ((volatile unsigned int*)(MMIO_BASE+0x0030001C))
#define EMMC_DATA           ((volatile unsigned int*)(MMIO_BASE+0x00300020))
#define EMMC_STATUS         ((volatile unsigned int*)(MMIO_BASE+0x00300024))
#define EMMC_CONTROL0       ((volatile unsigned int*)(MMIO_BASE+0x00300028))
#define EMMC_CONTROL1       ((volatile unsigned int*)(MMIO_BASE+0x0030002C))
#define EMMC_INTERRUPT      ((volatile unsigned int*)(MMIO_BASE+0x00300030))
#define EMMC_INT_MASK       ((volatile unsigned int*)(MMIO_BASE+0x00300034))
#define EMMC_INT_EN         ((volatile unsigned int*)(MMIO_BASE+0x00300038))
#define EMMC_CONTROL2       ((volatile unsigned int*)(MMIO_BASE+0x0030003C))
#define EMMC_SLOTISR_VER    ((volatile unsigned int*)(MMIO_BASE+0x003000FC))

#define CMD_GO_IDLE         0x00000000
#define CMD_ALL_SEND_CID    0x02010000
#define CMD_SEND_REL_ADDR   0x03020000
#define CMD_CARD_SELECT     0x07030000
#define CMD_SEND_IF_COND    0x08020000
#define CMD_STOP_TRANS      0x0C030000
#define CMD_READ_SINGLE     0x11220010
#define CMD_READ_MULTI      0x12220032
#define CMD_SET_BLOCKCNT    0x17020000
#define CMD_WRITE_SINGLE    0x18220000
#define CMD_WRITE_MULTI     0x19220022
#define CMD_APP_CMD         0x37000000
#define CMD_SET_BUS_WIDTH   (0x06020000|CMD_NEED_APP)
#define CMD_SEND_OP_COND    (0x29020000|CMD_NEED_APP)
#define CMD_SEND_SCR        (0x33220010|CMD_NEED_APP)

// command flags
#define CMD_NEED_APP        0x80000000
#define CMD_RSPNS_48        0x00020000
#define CMD_ERRORS_MASK     0xfff9c004
#define CMD_RCA_MASK        0xffff0000

#define INT_DATA_TIMEOUT    0x00100000
#define INT_CMD_TIMEOUT     0x00010000
#define INT_READ_RDY        0x00000020
#define INT_WRITE_RDY       0x00000010
#define INT_DATA_DONE       0x00000002
#define INT_CMD_DONE        0x00000001

#define INT_ERROR_MASK      0x017E8000

// COMMANDs

// STATUS register settings
#define SR_READ_AVAILABLE   0x00000800
#define SR_WRITE_AVAILABLE  0x00000400
#define SR_DAT_INHIBIT      0x00000002
#define SR_CMD_INHIBIT      0x00000001
#define SR_APP_CMD          0x00000020

// INTERRUPT register settings
#define INT_DATA_TIMEOUT    0x00100000
#define INT_CMD_TIMEOUT     0x00010000
#define INT_READ_RDY        0x00000020
#define INT_CMD_DONE        0x00000001

#define INT_ERROR_MASK      0x017E8000

// CONTROL register settings
#define C0_SPI_MODE_EN      0x00100000
#define C0_HCTL_HS_EN       0x00000004
#define C0_HCTL_DWITDH      0x00000002

#define C1_SRST_DATA        0x04000000
#define C1_SRST_CMD         0x02000000
#define C1_SRST_HC          0x01000000
#define C1_TOUNIT_DIS       0x000f0000
#define C1_TOUNIT_MAX       0x000e0000
#define C1_CLK_GENSEL       0x00000020
#define C1_CLK_EN           0x00000004
#define C1_CLK_STABLE       0x00000002
#define C1_CLK_INTLEN       0x00000001

// SLOTISR_VER values
#define HOST_SPEC_NUM       0x00ff0000
#define HOST_SPEC_NUM_SHIFT 16
#define HOST_SPEC_V3        2
#define HOST_SPEC_V2        1
#define HOST_SPEC_V1        0

// SCR flags
#define SCR_SD_BUS_WIDTH_4  0x00000400
#define SCR_SUPP_SET_BLKCNT 0x02000000
// added by my driver
#define SCR_SUPP_CCS        0x00000001

#define ACMD41_VOLTAGE      0x00ff8000
#define ACMD41_CMD_COMPLETE 0x80000000
#define ACMD41_CMD_CCS      0x40000000
#define ACMD41_ARG_HC       0x51ff8000

typedef struct {
    volatile uint32_t ARG2;
    volatile uint32_t BLKSIZECNT;
    volatile uint32_t ARG1;
    volatile uint32_t CMDTM;
    volatile uint32_t RESP0;
    volatile uint32_t RESP1;
    volatile uint32_t RESP2;
    volatile uint32_t RESP3;
    volatile uint32_t DATA;
    volatile uint32_t STATUS;
    volatile uint32_t CONTROL0;
    volatile uint32_t CONTROL1;
    volatile uint32_t INTERRUPT;
    volatile uint32_t IRPT_MASK;
    volatile uint32_t IRPT_EN;
    volatile uint32_t CONTROL2;
    volatile uint32_t FORCE_IRPT;
    volatile uint32_t BOOT_TIMEOUT;
    volatile uint32_t DBG_SEL;
    volatile uint32_t EXRDFIFO_CFG;
    volatile uint32_t EXRDFIFO_EN;
    volatile uint32_t TUNE_STEP;
    volatile uint32_t TUNE_STEPS_STD;
    volatile uint32_t TUNE_STEPS_DDR;
    volatile uint32_t SPI_INT_SPT;
    volatile uint32_t SLOTISR_VER;
} EMMC;

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(MMIO_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(MMIO_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(MMIO_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))

int sd_init();
int sd_readblock(unsigned int lba, void *buffer, unsigned int num);
int sd_writeblock(void *buffer, uint32_t lba, uint32_t num);
#endif