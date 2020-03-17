#include "shell.h"
#include "my_string.h"
#include "uart.h"

void run(char *command){
    if (!strcmp(command, "hello")){
        hello();
    }
    else if (!strcmp(command, "help")){
        help();
    }
    else if (!strcmp(command, "timestamp")){
        timestamp();
    }
    else {
        uart_puts("Error: command not found, try <help>.\n");
    }
}

void help(){
    uart_puts("<hello>: print Hello World!\n");
    uart_puts("---\n");
    uart_puts("<timestamp>: print current timestamp.\n");
    uart_puts("---\n");
    uart_puts("<help>: print all available commands.\n");
}

void hello(){
    uart_puts("Hello World!\n");
}

void timestamp(){
    int counter, freq;
    char res[30];
    char f_res[10];
    asm("mrs %0, cntpct_el0" : "=r"(counter));
    asm("mrs %0, cntfrq_el0" : "=r"(freq));

    //itoa((int)(counter/freq), res);
    ftoa((float)counter/freq, res, f_res);
    uart_send('[');
    uart_puts(res);
    uart_send('.');
    uart_puts(f_res);
    uart_puts("]\n");
}
