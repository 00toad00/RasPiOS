#include <driver/emmc.h>
#include <driver/timer.h>
#include <driver/gpio.h>
#include <peripherals.h>

unsigned long sd_scr[2], sd_ocr, sd_rca, sd_err, sd_hv;

int sd_status(unsigned int mask) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    int cnt = 500000; while((emmc->STATUS & mask) && !(emmc->INTERRUPT & INT_ERROR_MASK) && cnt--) delay(1000);
    return (cnt <= 0 || (emmc->INTERRUPT & INT_ERROR_MASK)) ? SD_ERROR : SD_OK;
}

/**
 * Wait for interrupt
 */
int sd_int(unsigned int mask) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    unsigned int r, m=mask | INT_ERROR_MASK;
    int cnt = 1000000; while(!(emmc->INTERRUPT & m) && cnt--) delay(1000);
    r=emmc->INTERRUPT;
    if(cnt<=0 || (r & INT_CMD_TIMEOUT) || (r & INT_DATA_TIMEOUT) ) { emmc->INTERRUPT=r; return SD_TIMEOUT; } else
    if(r & INT_ERROR_MASK) { emmc->INTERRUPT=r; return SD_ERROR; }
    emmc->INTERRUPT=mask;
    return 0;
}

/**
 * Send a command
 */
int sd_cmd(unsigned int code, unsigned int arg) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    int r=0;
    sd_err=SD_OK;
    if(code&CMD_NEED_APP) {
        r=sd_cmd(CMD_APP_CMD|(sd_rca?CMD_RSPNS_48:0),sd_rca);
        if(sd_rca && !r) { sd_err=SD_ERROR;return 0;}
        code &= ~CMD_NEED_APP;
    }
    if(sd_status(SR_CMD_INHIBIT)) { sd_err= SD_TIMEOUT;return 0;}
    emmc->INTERRUPT=emmc->INTERRUPT; emmc->ARG1=arg; emmc->CMDTM=code;
    if(code==CMD_SEND_OP_COND) delay(1000000); else
    if(code==CMD_SEND_IF_COND || code==CMD_APP_CMD) delay(100000);
    if((r=sd_int(INT_CMD_DONE))) {sd_err=r;return 0;}
    r=emmc->RESP0;
    if(code==CMD_GO_IDLE || code==CMD_APP_CMD) return 0; else
    if(code==(CMD_APP_CMD|CMD_RSPNS_48)) return r&SR_APP_CMD; else
    if(code==CMD_SEND_OP_COND) return r; else
    if(code==CMD_SEND_IF_COND) return r==(int)arg? SD_OK : SD_ERROR; else
    if(code==CMD_ALL_SEND_CID) {r|=emmc->RESP3; r|=emmc->RESP2; r|=emmc->RESP1; return r; } else
    if(code==CMD_SEND_REL_ADDR) {
        sd_err=(((r&0x1fff))|((r&0x2000)<<6)|((r&0x4000)<<8)|((r&0x8000)<<8))&CMD_ERRORS_MASK;
        return r&CMD_RCA_MASK;
    }
    return r&CMD_ERRORS_MASK;
    // make gcc happy
    return 0;
}

int sd_readblock(uint32_t lba, void *buffer, uint32_t num) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    int r,c=0,d;
    if(num<1) num=1;
    if(sd_status(SR_DAT_INHIBIT)) {sd_err=SD_TIMEOUT; return 0;}
    unsigned int *buf=(unsigned int *)buffer;
    if(sd_scr[0] & SCR_SUPP_CCS) {
        if(num > 1 && (sd_scr[0] & SCR_SUPP_SET_BLKCNT)) {
            sd_cmd(CMD_SET_BLOCKCNT,num);
            if(sd_err) return 0;
        }
        emmc->BLKSIZECNT = (num << 16) | 512;
        sd_cmd(num == 1 ? CMD_READ_SINGLE : CMD_READ_MULTI,lba);
        if(sd_err) return 0;
    } else {
        emmc->BLKSIZECNT = (1 << 16) | 512;
    }
    while( c < (int)num ) {
        if(!(sd_scr[0] & SCR_SUPP_CCS)) {
            sd_cmd(CMD_READ_SINGLE,(lba+c)*512);
            if(sd_err) return 0;
        }
        if((r=sd_int(INT_READ_RDY))){sd_err=r;return 0;}
        for(d=0;d<128;d++) buf[d] = emmc->DATA;
        c++; buf+=128;
    }
    if( num > 1 && !(sd_scr[0] & SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SCR_SUPP_CCS)) sd_cmd(CMD_STOP_TRANS,0);
    return sd_err!=SD_OK || c!=(int)num? 0 : num*512;
}

int sd_writeblock(void *buffer, uint32_t lba, uint32_t num) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    int r,c=0,d;
    if(num<1) num=1;
    if(sd_status(SR_DAT_INHIBIT | SR_WRITE_AVAILABLE)) {sd_err=SD_TIMEOUT; return 0;}
    uint32_t *buf=(uint32_t*)buffer;
    if(sd_scr[0] & SCR_SUPP_CCS) {
        if(num > 1 && (sd_scr[0] & SCR_SUPP_SET_BLKCNT)) {
            sd_cmd(CMD_SET_BLOCKCNT,num);
            if(sd_err) return 0;
        }
        emmc->BLKSIZECNT = (num << 16) | 512;
        sd_cmd(num == 1 ? CMD_WRITE_SINGLE : CMD_WRITE_MULTI,lba);
        if(sd_err) return 0;
    } else {
        emmc->BLKSIZECNT = (1 << 16) | 512;
    }
    while( c < (int)num ) {
        if(!(sd_scr[0] & SCR_SUPP_CCS)) {
            sd_cmd(CMD_WRITE_SINGLE,(lba+c)*512);
            if(sd_err) return 0;
        }
        if((r=sd_int(INT_WRITE_RDY))){return 0;}
        for(d=0;d<128;d++) emmc->DATA = buf[d];
        c++; buf+=128;
    }
    if((r=sd_int(INT_DATA_DONE))){return 0;}
    if( num > 1 && !(sd_scr[0] & SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SCR_SUPP_CCS)) sd_cmd(CMD_STOP_TRANS,0);
    return sd_err!=SD_OK || c!=(int)num? 0 : num*512;
}

/**
 * set SD clock to frequency in Hz
 */
int sd_clk(unsigned int f) {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    unsigned int d,c=41666666/f,x,s=32,h=0;
    int cnt = 100000;
    while((emmc->STATUS & (SR_CMD_INHIBIT|SR_DAT_INHIBIT)) && cnt--) delay(1000);
    if(cnt<=0) {
        return SD_ERROR;
    }

    emmc->CONTROL1 &= ~C1_CLK_EN; delay(10000);
    x=c-1; if(!x) s=0; else {
        if(!(x & 0xffff0000u)) { x <<= 16; s -= 16; }
        if(!(x & 0xff000000u)) { x <<= 8;  s -= 8; }
        if(!(x & 0xf0000000u)) { x <<= 4;  s -= 4; }
        if(!(x & 0xc0000000u)) { x <<= 2;  s -= 2; }
        if(!(x & 0x80000000u)) { x <<= 1;  s -= 1; }
        if(s>0) s--;
        if(s>7) s=7;
    }
    if(sd_hv>HOST_SPEC_V2) d=c; else d=(1<<s);
    if(d<=2) {d=2;s=0;}
    if(sd_hv>HOST_SPEC_V2) h=(d&0x300)>>2;
    d=(((d&0x0ff)<<8)|h);
    emmc->CONTROL1=(emmc->CONTROL1&0xffff003f)|d; delay(10*1000);
    emmc->CONTROL1 |= C1_CLK_EN; delay(10000);
    cnt=10000; while(!(emmc->CONTROL1 & C1_CLK_STABLE) && cnt--) delay(10*1000);
    if(cnt<=0) {
        return SD_ERROR;
    }
    return SD_OK;
}

/**
 * initialize EMMC to read SDHC card
 */
int sd_init() {
    EMMC *emmc = (EMMC*)EMMC_BASE;
    GPIO *gpio = (GPIO*)GPIO_BASE;
    long r,cnt,ccs=0;
    // GPIO_CD
    r=gpio->FSEL4; r&=~(7<<(7*3)); gpio->FSEL4=r;
    gpio->PUD=2; wait_cycles(150); gpio->PUDCLK0=(1<<15); wait_cycles(150); gpio->PUD=0; gpio->PUDCLK1=0;
    r=gpio->HEN1; r|=1<<15; gpio->HEN1=r;

    // GPIO_CLK, GPIO_CMD
    r=gpio->FSEL4; r|=(7<<(8*3))|(7<<(9*3)); gpio->FSEL4=r;
    gpio->PUD=2; wait_cycles(150); gpio->PUDCLK1=(1<<16)|(1<<17); wait_cycles(150); gpio->PUD=0; gpio->PUDCLK1=0;

    // GPIO_DAT0, GPIO_DAT1, GPIO_DAT2, GPIO_DAT3
    r=gpio->FSEL5; r|=(7<<(0*3)) | (7<<(1*3)) | (7<<(2*3)) | (7<<(3*3)); gpio->FSEL5=r;
    gpio->PUD=2; wait_cycles(150);
    gpio->PUDCLK1=(1<<18) | (1<<19) | (1<<20) | (1<<21);
    wait_cycles(150); gpio->PUD=0; gpio->PUDCLK1=0;

    sd_hv = (emmc->SLOTISR_VER & HOST_SPEC_NUM) >> HOST_SPEC_NUM_SHIFT;
    // Reset the card.
    emmc->CONTROL0 = 0; emmc->CONTROL1 |= C1_SRST_HC;
    cnt=10000; do{delay(10000);} while( (emmc->CONTROL1 & C1_SRST_HC) && cnt-- );
    if(cnt<=0) {
        return SD_ERROR;
    }
    emmc->CONTROL1 |= C1_CLK_INTLEN | C1_TOUNIT_MAX;
    delay(10*1000);
    // Set clock to setup frequency.
    if((r=sd_clk(400000))) return r;
    emmc->IRPT_EN   = 0xffffffff;
    *EMMC_INT_MASK = 0xffffffff;
    sd_scr[0]=sd_scr[1]=sd_rca=sd_err=0;
    sd_cmd(CMD_GO_IDLE,0);
    if(sd_err) return sd_err;

    sd_cmd(CMD_SEND_IF_COND,0x000001AA);
    if(sd_err) return sd_err;
    cnt=6; r=0; while(!(r&ACMD41_CMD_COMPLETE) && cnt--) {
        wait_cycles(400);
        r=sd_cmd(CMD_SEND_OP_COND,ACMD41_ARG_HC);
        if((long)sd_err!=SD_TIMEOUT && sd_err!=SD_OK ) {
            return sd_err;
        }
    }
    if(!(r&ACMD41_CMD_COMPLETE) || !cnt ) return SD_TIMEOUT;
    if(!(r&ACMD41_VOLTAGE)) return SD_ERROR;
    if(r&ACMD41_CMD_CCS) ccs=SCR_SUPP_CCS;

    sd_cmd(CMD_ALL_SEND_CID,0);

    sd_rca = sd_cmd(CMD_SEND_REL_ADDR,0);
    if(sd_err) return sd_err;

    if((r=sd_clk(25000000))) return r;

    sd_cmd(CMD_CARD_SELECT,sd_rca);
    if(sd_err) return sd_err;

    if(sd_status(SR_DAT_INHIBIT)) return SD_TIMEOUT;
    emmc->BLKSIZECNT = (1<<16) | 8;
    sd_cmd(CMD_SEND_SCR,0);
    if(sd_err) return sd_err;
    if(sd_int(INT_READ_RDY)) return SD_TIMEOUT;

    r=0; cnt=100000; while(r<2 && cnt) {
        if( emmc->STATUS & SR_READ_AVAILABLE )
            sd_scr[r++] = emmc->DATA;
        else
            delay(1000);
    }
    if(r!=2) return SD_TIMEOUT;
    if(sd_scr[0] & SCR_SD_BUS_WIDTH_4) {
        sd_cmd(CMD_SET_BUS_WIDTH,sd_rca|2);
        if(sd_err) return sd_err;
        emmc->CONTROL0 |= C0_HCTL_DWITDH;
    }
    // add software flag
    sd_scr[0]&=~SCR_SUPP_CCS;
    sd_scr[0]|=ccs;
    return SD_OK;
}