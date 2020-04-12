#include "frame_buffer.h"
#include "exce.h"
#include "mailbox.h"
#include "printf.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    unsigned long el;
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    asm volatile ("mrs %0, CurrentEL" : "=r" (el));
    printf("Current EL is: %u\n", (el>>2)&3);

    run_shell();
    return -1;
}
