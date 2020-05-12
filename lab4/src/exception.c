#include "uart.h"
#include "sys.h"

void
_el2_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    uart_puts("Exception Class: 0x");
    uart_hex(esr>>(32-6));
    uart_puts("\r\nInstruction Specific syndrome: 0x");
    uart_hex(esr&&(0x100));
    uart_puts("\r\nException return address: 0x");
    uart_hex(elr>>32);
    uart_hex(elr);
    uart_puts("\r\n");
}

void
_el1_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far, unsigned long sys_call, unsigned long arg)
{
    unsigned int ec, iss;
    
    ec = esr >> 26;
    iss = esr & 0xffffff;

    if(ec == 0x15) {
        if(iss == 0x1) {
            // svc #1
            uart_puts("Exception Class: 0x");
            uart_hex(ec);
            uart_puts("\r\nInstruction Specific syndrome: 0x");
            uart_hex(esr && (0xffffff));
            uart_puts("\r\nException return address: 0x");
            uart_hex(elr>>32);
            uart_hex(elr);
            uart_puts("\r\n");
        }
        else if(iss == 0x0) {
            // svc #0 for time interrupts
            // uart_puts("#0\r\n");
            uart_print_int(sys_call);
            uart_puts("\r\n");
            if(sys_call == SYS_WRITE_NUMBER) {
                do_uart_write(arg);
            } else if(sys_call == SYS_READ_NUMBER) {
                do_uart_read(arg);
            } else if(sys_call == SYS_EXEC_NUMBER) {
                uart_puts("exec\r\n");
                do_exec(arg);
            } else if(sys_call == SYS_FORK_NUMBER) {
                
            } else if(sys_call == SYS_EXIT_NUMBER) {
                
            } else {
                local_timer_init();
			    core_timer_init();
            }
            // uart_print_int(sys_call);
            // local_timer_init();
			// core_timer_init();
        }
    }
}

void
_context_switch_msg()
{
    uart_puts("context switching...\r\n");
}

void
_not_implement()
{
    uart_puts("Not implement.\r\n");
}
