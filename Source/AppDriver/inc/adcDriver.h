/*
adcDriver.h

������ֻ�õ�һ��AD�����¶ȿ���ϵͳ�е�AD��
*/

#ifndef _ADC_DRIVER_H
#define _ADC_DRIVER_H

#include "stm32f10x_gpio.h"

#define ADC_PIN_TEM				GPIO_Pin_0		/* PC0������1 */			

#define CHANNEL_NUM 		1


/*���¿��ƽṹ��*/
typedef struct _ADC_DATA
{
	u16 tem; 	  	
}ADC_DATA;

extern ADC_DATA g_ad;

/*
* AD��ʼ��
* ���������š�DMA��AD
*/
void adcInit(void);

#endif


