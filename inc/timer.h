#ifndef	_TIMER_H
#define	_TIMER_H

#include "peri_base.h"

void core_timer_enable();
void core_timer_handler();
void local_timer_init();
void local_timer_handler();

#endif  /*_TIMER_H */