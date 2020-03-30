#ifndef __TIME_H_
#define __TIME_H_

void delay(int32_t count);

uint32_t mm_freq();

uint64_t mm_ticks();

void get_timestamp(char* timestamp);

#endif
