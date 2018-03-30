/*
temDrive.h

����ϵͳ�ײ�����ʵ��
��Ҫ���ڶ�����ȡ��������ţ�ʵ�ּ�����������ơ�

*/


#ifndef _TEMPERATURE_DRIVER_H
#define _TEMPERATURE_DRIVER_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_type.h"


#define TEM_NUM 	1 	/* �¿���·�� */

enum TEM_X{ TEM1 = 0};



/* �¿�1�������� */
#define TEM_GPIOX 			GPIOD
#define TEM_PIN_C1 			GPIO_Pin_7 		//PD7
#define TEM_PIN_C2 			GPIO_Pin_6 		//PD6
#define TEM_PIN_SD 			GPIO_Pin_5 		//PD5

/* IO������ƽṹ�� */
typedef struct _TEM_IO_CTRL
{
	u16 C1;
	u16 C2;
	u16 SD;
}TEM_IO_CTRL;


extern TEM_IO_CTRL g_tem_io[TEM_NUM];


/* 
��ʼ��
�������¶ȿ�����������ΪOUT_PPģʽ
*/
void temPinInit(void);

/* 
����
��������Ĳ���ʹ��Ӧ���¿ؼ���
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temHeat(u8 p_id);

/* 
����
��������Ĳ���ʹ��Ӧ���¿�����
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temCool(u8 p_id);


/* 
ֹͣ
��������Ĳ���ʹ��Ӧ���¿�ֹͣ
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temStop(u8 p_id);


#endif


