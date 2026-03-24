#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define SPI_CS_LEN_LONG (1<<25)
#define SPI_CS_DMA_LEN  (1<<24)
#define SPI_CS_CSPOL2   (1<<23)
#define SPI_CS_CSPOL1   (1<<22)
#define SPI_CS_CSPOL0   (1<<21)
#define SPI_CS_RXF      (1<<20)
#define SPI_CS_RXR      (1<<19)
#define SPI_CS_TXD      (1<<18)
#define SPI_CS_RXD      (1<<17)
#define SPI_CS_DONE     (1<<16)
#define SPI_CS_TE_EN    (1<<15)
#define SPI_CS_LMONO    (1<<14)
#define SPI_CS_LEN      (1<<13)
#define SPI_CS_REN      (1<<12)
#define SPI_CS_ADCS     (1<<11)
#define SPI_CS_INTR     (1<<10)
#define SPI_CS_INTD     (1<<9)
#define SPI_CS_DMAEN    (1<<8)
#define SPI_CS_TA       (1<<7)
#define SPI_CS_CLEAR_RX (1<<5)
#define SPI_CS_CLEAR_TX (1<<4)
#define SPI_CS_CPOL     (1<<3)
#define SPI_CS_CPHA     (1<<2)
#define SPI_CS_CS_10    (2)
#define SPI_CS_CS_01    (1)
#define SPI_CS_CS_00    (0)

typedef struct {
    volatile uint32_t CS;
    volatile uint32_t FIFO;
    volatile uint32_t CLK;
    volatile uint32_t DLEN;
    volatile uint32_t LTOH;
    volatile uint32_t DC;
} SPI0;

void spi0_init();
uint8_t spi0_transfer(uint8_t data);
void spi0_cs_low();
void spi0_cs_high();

#endif