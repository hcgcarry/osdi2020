#include "sys.h"
#include "uart.h"
#include "string_util.h"
#include "timer.h"
#include "irq.h"
#include "task.h"

int sys_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss)
{
  /* obsolete */
  char string_buff[0x20];

  uart_puts("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Exception class (EC): ");
  string_ulonglong_to_hex_char(string_buff, exception_class);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Instruction specific syndrome (ISS): ");
  string_ulonglong_to_hex_char(string_buff, exception_iss);
  uart_puts(string_buff);
  uart_putc('\n');

  return 0;
}

int sys_timer_int(void)
{
  /* obsolete */
  static int core_timer_enabled = 0;
  static int local_timer_enabled = 0;

  if(!core_timer_enabled)
  {
    timer_enable_core_timer();
    core_timer_enabled = 1;
  }
  if(!local_timer_enabled)
  {
    timer_enable_and_set_local_timer();
  }
  timer_set_core_timer_sec(CORE_TIMER_SECS);

  return 0;
}

int sys_uart_puts(char * string)
{
  uart_puts(string);
  return 0;
}

int sys_uart_gets(char * string, char delimiter, unsigned length)
{
  irq_int_enable();
  uart_gets(string, delimiter, length);
  return 0;
}

int sys_exec(void(*start_func)())
{
  task_do_exec(start_func);
  return 0;
}


