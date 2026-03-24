#include <driver/gpio.h>
#include <peripherals.h>

void gpio_set_mode(uint8_t pin, GPIOMode mode) {
    GPIO *gpio = (GPIO*)GPIO_BASE;
    if (mode != GPF_INPUT) {
        if (pin <= 9) {
            gpio->FSEL0 &= ~(0b111 << 3*pin);
            gpio->FSEL0 |= (mode << 3*pin);
        } else if (pin >= 10 && pin <= 19) {
            gpio->FSEL1 &= ~(0b111 << 3*(pin-10));
            gpio->FSEL1 |= (mode << 3*(pin-10));
        } else if (pin >= 20 && pin <= 29) {
            gpio->FSEL2 &= ~(0b111 << 3*(pin-20));
            gpio->FSEL2 |= (mode << 3*(pin-20));
        } else if (pin >= 30 && pin <= 39) {
            gpio->FSEL3 &= ~(0b111 << 3*(pin-30));
            gpio->FSEL3 |= (mode << 3*(pin-30));
        } else if (pin >= 40 && pin <= 49) {
            gpio->FSEL4 &= ~(0b111 << 3*(pin-40));
            gpio->FSEL4 |= (mode << 3*(pin-40));
        } else if (pin >= 50 && pin <= 53) {
            gpio->FSEL5 &= ~(0b111 << 3*(pin-50));
            gpio->FSEL5 |= (mode << 3*(pin-50));
        }
    } else if (mode == GPF_INPUT) {
        if (pin <= 9) {
            gpio->FSEL0 &= ~(0b111 << 3*pin);
        } else if (pin >= 10 && pin <= 19) {
            gpio->FSEL1 &= ~(0b111 << 3*(pin-10));
        } else if (pin >= 20 && pin <= 29) {
            gpio->FSEL2 &= ~(0b111 << 3*(pin-20));
        } else if (pin >= 30 && pin <= 39) {
            gpio->FSEL3 &= ~(0b111 << 3*(pin-30));
        } else if (pin >= 40 && pin <= 49) {
            gpio->FSEL4 &= ~(0b111 << 3*(pin-40));
        } else if (pin >= 50 && pin <= 53) {
            gpio->FSEL5 &= ~(0b111 << 3*(pin-50));
        }     
    }
    return;
}

void gpio_set_status(uint8_t pin, GPIOStatus status) {
    GPIO *gpio = (GPIO*)GPIO_BASE;
    if (status == GPIO_HIGH) {
        if (pin <= 31) { gpio->SET0 = (1 << pin); }
        else if (pin >= 32 && pin <= 21) { gpio->SET1 = (1 << (pin-32)); }
    } else if (status == GPIO_LOW) {
        if (pin <= 31) { gpio->CLR0 = (1 << pin); }
        else if (pin >= 32 && pin <= 21) { gpio->CLR1 = (1 << (pin-32)); }        
    }
    return;
}

uint8_t gpio_get_status(uint8_t pin) {
    GPIO *gpio = (GPIO*)GPIO_BASE;
    if (pin <= 31) {
        if (gpio->LEV0 & (1 << pin)) { return GPIO_HIGH; }
        return GPIO_LOW;
    }
    else if (pin >= 32 && pin <= 21) {
        if (gpio->LEV1 & (1 << pin)) { return GPIO_HIGH; }
        return GPIO_LOW;
    }
    return GPIO_LOW;
}