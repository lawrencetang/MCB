#ifndef __TIMER4_H
#define __TIMER4_H

#include "stm32f10x_type.h"

void Timer4Init(u16 l_u16SMPeriod);

void Timer4Start(void);

void Timer4Stop(void);

void Timer4Set(u16 r_u8BstInter);

#endif

