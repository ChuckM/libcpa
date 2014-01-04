/*
 * Copyright (c) 2013 - 2014 Charles McManis (cmcmanis@mcmanis.com)
 *
 * All Rights Reserved.
 *
 * GPIO helper functions
 *
 * I exploit the memory map on the STM32 here in a couple of ways.
 * One since the ENUM will make the pin values 0 - 127, I can do
 * basic math on them to pull out which bit they represent with
 *  bit = pin % 16; 
 * I can pull out which GPIO base address they represent by
 * adding dividing by 16 (>> 4) and then multiplying by the increase
 * in offsets (0x400).
 * And I can compute the clock bit in the AHB1ENR register since GPIOA
 * is 0, and GPIOI is 8, with 1 << (pin >> 4).
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "cpa.h"

static uint32_t enabled_clocks = 0;

const uint32_t clock_constants[] = {
    RCC_AHB1ENR_IOPAEN,
    RCC_AHB1ENR_IOPBEN,
    RCC_AHB1ENR_IOPCEN,
    RCC_AHB1ENR_IOPDEN,
    RCC_AHB1ENR_IOPEEN,
    RCC_AHB1ENR_IOPFEN,
    RCC_AHB1ENR_IOPGEN,
    RCC_AHB1ENR_IOPHEN,
    RCC_AHB1ENR_IOPIEN
};

void
gpio_enable_clock(enum GPIO_PORT_PIN pin) {
    uint32_t clock;
    clock = clock_constants[((uint32_t) pin) >> 4];
    if ((enabled_clocks & clock) == 0) {
        rcc_peripheral_enable_clock(&RCC_AHB1ENR, clock);
        enabled_clocks |= clock;
    }
}
