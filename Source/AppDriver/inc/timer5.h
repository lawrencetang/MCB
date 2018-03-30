/*
timer5.h

timer5初始化为PWM输出模式

*/



#ifndef __TIMER5_H
#define __TIMER5_H

#include "stm32f10x_type.h"


/*
* 配置定时器5的周期
* period：计数周期数（1us），其中计数时钟为1MHz，即1us
* 基本功能是：将定时器5配置为向上计数
*/
void tim5SetPeriod(u16 period);


/*
* 设置定时器5某通道PWM的高电平时间
* channel：通道号
* highPulse：高电平时间，与自动装载值配合，高电平占空比 = highPulse / 自动装载值
*/
void tim5ConfigPWM(u8 channel, u16 highPulse);


/*
* 启动定时器5
*/
void tim5StartCounter(void);


#endif

