/*
光检测
light_detect.h
*/

#ifndef _MY_AD_H
#define _MY_AD_H

#include "stm32f10x_gpio.h"

/*光检测结构体*/
typedef struct _LIGHT_DET
{
	u8 start;   /* 启停，0 表示停止检测，1 表示启动检测 */
	u16 cnt; 	/* 检测次数计数，主要用于三个参数的对齐 */

	u32 i0;
	u32 i1;
	u16 f;

	OS_EVENT *Event; 	  	
}LIGHT_DET;






#endif