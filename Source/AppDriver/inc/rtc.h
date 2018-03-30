#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x_type.h"

void RTC_Configuration(void);

void Time_Adjust(u32 r_u32Unixtime);

#endif
