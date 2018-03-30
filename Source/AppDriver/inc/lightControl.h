/*
lightControl.h
氙灯控制与积分时间控制。
氙灯和CMOS的积分时间由定时器控制
定时器固定周期为100us，点灯周期固定为15ms。
*/

#ifndef _LIGHTCONTROL_H
#define _LIGHTCONTROL_H

#include "stm32f10x_type.h"


#define TIMER_IRQ_FREQUENT 		10000 	/* 定时器中断频率，默认为10KHz，周期为0.1ms */	
#define LIGHT_MAX_FREQUENT  	80 		/* 点灯控制的最大频率，默认为100Hz */
#define ST_DEFAULT_TIME 		0.01 	/* 默认的积分时间，单位为秒 */
#define ST_DELAY				5 		/* 点灯到开始积分的延时 */ 

#define FrequentToIrqCnt(f)  	(TIMER_IRQ_FREQUENT/f)			/* 频率转换为对应的定时器中断次数 */
#define IrqCntToTime(c) 		(1.0*c/TIMER_IRQ_FREQUENT) 		/* 定时器中断次数转换为对应的时间，单位为秒 */	
#define TimeToIrqCnt(t)			(t*TIMER_IRQ_FREQUENT) 			/* 积分时间转换为对应的定时器中断次数，时间单位为秒 */

/* 点灯控制全局变量 */
typedef struct _lightCtrl
{
	u8 flag; 			/* 关灯标识，1 点灯， 0 关灯 */
	u32 counter;   		/* 点灯控制定时器中断计数器 */
	u32 totalCnt;   	/* 每点一次灯要求定时器中断的次数 */
}lightCtrl;

extern lightCtrl g_lightCtrl;



/* 积分时间控制全局变量 */
typedef struct _cmosCtrl
{
	u8 flag; 			/* CMOS一次采集标识，1 本次采集未完成， 0 本次采数完成。在积分开始时置1，在数据读完时置0 */	
	u32 counter;   		/* 积分控制定时器中断计数器 */
	u32 totalCnt;  		/* 积分时间对应的定时器中断次数 */
}cmosCtrl;

extern cmosCtrl g_cmosCtrl;


#endif


