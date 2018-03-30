/*
lightControl.h
믵ƿ��������ʱ����ơ�
믵ƺ�CMOS�Ļ���ʱ���ɶ�ʱ������
��ʱ���̶�����Ϊ100us��������ڹ̶�Ϊ15ms��
*/

#ifndef _LIGHTCONTROL_H
#define _LIGHTCONTROL_H

#include "stm32f10x_type.h"


#define TIMER_IRQ_FREQUENT 		10000 	/* ��ʱ���ж�Ƶ�ʣ�Ĭ��Ϊ10KHz������Ϊ0.1ms */	
#define LIGHT_MAX_FREQUENT  	80 		/* ��ƿ��Ƶ����Ƶ�ʣ�Ĭ��Ϊ100Hz */
#define ST_DEFAULT_TIME 		0.01 	/* Ĭ�ϵĻ���ʱ�䣬��λΪ�� */
#define ST_DELAY				5 		/* ��Ƶ���ʼ���ֵ���ʱ */ 

#define FrequentToIrqCnt(f)  	(TIMER_IRQ_FREQUENT/f)			/* Ƶ��ת��Ϊ��Ӧ�Ķ�ʱ���жϴ��� */
#define IrqCntToTime(c) 		(1.0*c/TIMER_IRQ_FREQUENT) 		/* ��ʱ���жϴ���ת��Ϊ��Ӧ��ʱ�䣬��λΪ�� */	
#define TimeToIrqCnt(t)			(t*TIMER_IRQ_FREQUENT) 			/* ����ʱ��ת��Ϊ��Ӧ�Ķ�ʱ���жϴ�����ʱ�䵥λΪ�� */

/* ��ƿ���ȫ�ֱ��� */
typedef struct _lightCtrl
{
	u8 flag; 			/* �صƱ�ʶ��1 ��ƣ� 0 �ص� */
	u32 counter;   		/* ��ƿ��ƶ�ʱ���жϼ����� */
	u32 totalCnt;   	/* ÿ��һ�ε�Ҫ��ʱ���жϵĴ��� */
}lightCtrl;

extern lightCtrl g_lightCtrl;



/* ����ʱ�����ȫ�ֱ��� */
typedef struct _cmosCtrl
{
	u8 flag; 			/* CMOSһ�βɼ���ʶ��1 ���βɼ�δ��ɣ� 0 ���β�����ɡ��ڻ��ֿ�ʼʱ��1�������ݶ���ʱ��0 */	
	u32 counter;   		/* ���ֿ��ƶ�ʱ���жϼ����� */
	u32 totalCnt;  		/* ����ʱ���Ӧ�Ķ�ʱ���жϴ��� */
}cmosCtrl;

extern cmosCtrl g_cmosCtrl;


#endif


