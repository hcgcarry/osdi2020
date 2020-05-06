#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_CNTFRQ      0
#define SYS_GET_CNTPCT      1
#define SYS_UART_READ       2
#define SYS_UART_WRITE      3
#define SYS_EXEC            4
#define SYS_FORK            5
#define SYS_EXIT            6
#define SYS_KILL            7

#endif

#ifndef IN_ASM

#include "stdint.h"

uint64_t get_cntfrq();
uint64_t get_cntpct();
uint32_t uart_read(char buf[], uint32_t size);
uint32_t uart_write(const char buf[], uint32_t size);
int exec(void(*func)());
int fork();
void exit(int status);
int kill(uint32_t pid, int sig);

#endif
