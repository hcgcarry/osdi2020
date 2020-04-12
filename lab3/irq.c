#include "irq.h"
#include "uart.h"
#include "timer.h"

void irq_el2_enable(void)
{
  // Clear interrupt mask for d, a, (i), f
  asm volatile("msr daifclr, #0x2");
}

void irq_el2_handler(void)
{
  uart_puts("ARM core time interrupt received\n");
  timer_expire_core_timer();
  return;
}

