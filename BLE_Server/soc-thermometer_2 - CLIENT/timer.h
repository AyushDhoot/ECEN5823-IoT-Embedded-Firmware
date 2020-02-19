

#ifndef TIMER_H_
#define TIMER_H_

#include "em_timer.h"
#include "em_cmu.h"

void Timer0_setup(void);
void Timer0_IRQHandler(void);

#endif  TIMER_H_
