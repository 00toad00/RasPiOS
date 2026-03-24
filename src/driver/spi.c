#include <driver/spi.h>
#include <driver/gpio.h>
#include <peripherals.h>

void spi0_init() {
    SPI0 *spi0 = (SPI0*)SPI0_BASE;
    for (uint8_t i = 8; i < 12; i++) { gpio_set_mode(i, GPF_ALT0); }
    spi0->CS = 0;
    spi0->CS = SPI_CS_CLEAR_RX | SPI_CS_CLEAR_TX;
    spi0->CLK = 500;
}

uint8_t spi0_transfer(uint8_t data) {
    SPI0 *spi0 = (SPI0*)SPI0_BASE;
    spi0->CS |= SPI_CS_TA;
    while (!(spi0->CS & SPI_CS_TXD));
    spi0->FIFO = data;
    while (!(spi0->CS & SPI_CS_RXD));
    uint8_t r = spi0->FIFO;
    while(!(spi0->CS & SPI_CS_DONE));
    spi0->CS &= ~SPI_CS_TA;
    return r;
}

void spi0_cs_low() {
    SPI0 *spi0 = (SPI0*)SPI0_BASE;
    spi0->CS = (spi0->CS & ~3) | 0;
}

void spi0_cs_high() {
    SPI0 *spi0 = (SPI0*)SPI0_BASE;
    spi0->CS = (spi0->CS & ~3) | 3;
}