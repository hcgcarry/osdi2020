#ifndef	__UTIL_H__
#define	__UTIL_H__

extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);

#define delay(t) do{int r=t; while(r--) { __asm__ volatile("nop"); }}while(0)

#endif
