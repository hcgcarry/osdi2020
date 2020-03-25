/*
 * mm.h
 * Provides a basic API to interact with rpi3's GPIOs
 * 
 * source: https://jsandler18.github.io/tutorial/boot.html
 */

#ifndef __MM_H_
#define __MM_H_

#define MMIO_BASE        0x3F000000
#define MAILBOX_BASE     ((volatile unsigned int*)(MMIO_BASE + 0xb880))

#define MAILBOX_READ     ((volatile unsigned int*)(MAILBOX_BASE + 0x0))
#define MAILBOX_STATUS   ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE    ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

#define MAILBOX_EMPTY    0x40000000
#define MAILBOX_FULL     0x80000000

#define GPFSEL1          ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPSET0           ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPCLR0           ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPPUD            ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0        ((volatile unsigned int*)(MMIO_BASE+0x00200098))

#include "types.h"

static inline void mm_write(volatile uint32_t *reg, uint32_t data) {
    *(reg) = data;
}

static inline uint32_t mm_read(volatile uint32_t *reg) {
    return *(reg);
}

#endif
