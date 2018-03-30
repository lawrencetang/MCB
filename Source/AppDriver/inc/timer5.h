/*
timer5.h

timer5��ʼ��ΪPWM���ģʽ

*/



#ifndef __TIMER5_H
#define __TIMER5_H

#include "stm32f10x_type.h"


/*
* ���ö�ʱ��5������
* period��������������1us�������м���ʱ��Ϊ1MHz����1us
* ���������ǣ�����ʱ��5����Ϊ���ϼ���
*/
void tim5SetPeriod(u16 period);


/*
* ���ö�ʱ��5ĳͨ��PWM�ĸߵ�ƽʱ��
* channel��ͨ����
* highPulse���ߵ�ƽʱ�䣬���Զ�װ��ֵ��ϣ��ߵ�ƽռ�ձ� = highPulse / �Զ�װ��ֵ
*/
void tim5ConfigPWM(u8 channel, u16 highPulse);


/*
* ������ʱ��5
*/
void tim5StartCounter(void);


#endif

