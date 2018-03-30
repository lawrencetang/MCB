#ifndef __TIMER3_H
#define __TIMER3_H

#include "stm32f10x_type.h"


#define TIMER3_PERIOD 		0.00003  	/* TIMER3定时间隔，单位：秒 */



void Timer3Init(void);

void Timer3Start(void);

void Timer3Stop(void);

void Timer3Set(void);

#endif

