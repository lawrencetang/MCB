/*
����
light_detect.h
*/

#ifndef _MY_AD_H
#define _MY_AD_H

#include "stm32f10x_gpio.h"

/*����ṹ��*/
typedef struct _LIGHT_DET
{
	u8 start;   /* ��ͣ��0 ��ʾֹͣ��⣬1 ��ʾ������� */
	u16 cnt; 	/* ��������������Ҫ�������������Ķ��� */

	u32 i0;
	u32 i1;
	u16 f;

	OS_EVENT *Event; 	  	
}LIGHT_DET;






#endif