/*
adcDriver.h

本工程只用到一个AD，即温度控制系统中的AD。
*/

#ifndef _ADC_DRIVER_H
#define _ADC_DRIVER_H

#include "stm32f10x_gpio.h"

#define ADC_PIN_TEM				GPIO_Pin_0		/* PC0，恒温1 */			

#define CHANNEL_NUM 		1


/*恒温控制结构体*/
typedef struct _ADC_DATA
{
	u16 tem; 	  	
}ADC_DATA;

extern ADC_DATA g_ad;

/*
* AD初始化
* 包括：引脚、DMA、AD
*/
void adcInit(void);

#endif


