
/*
absEncoder.h
����ֵ������
*/

#ifndef _ABS_ENCODER_H
#define _ABS_ENCODER_H

#include "stm32f10x_type.h"

/* ���������� */
#define ABS_ENC_NUM 		2


/* ����ֵ������ */
typedef struct _absEncoder
{
	u16 address; 					/* EEPROM�д���߼����ĵ�ַ */
	float absAngle;   		/* ���������ԽǶ�ֵ */
	float logZero;
	float logAngle;
}absEncoder;

extern absEncoder g_absEnc[ABS_ENC_NUM];

/*
* ����ֵ��������ʼ��
*/
void absEncInit(void);


/*
* ����ǰ�Ƕ�
*/
float absEncGetAngle(u8 num);


/*
* ���½Ƕ�
* ʵ���ϵ��øú������½Ƕ�ʱ���õ�������һ�θ��µĽ������Ϊ���ǶȲ�����Ҫ�ȴ���Ϊ�˲��ȴ���ֱ��ȡ���ݡ�
*/
void absEncUpdateAngle(u8 num);


/*
* д�߼����
*/
u8 absEncWriteZero(u8 num, float angle);


/*
* ���߼����
*/
float absEncReadZero(u8 num);


/*
* ���õ�ǰ�Ƕ�Ϊ�߼����
* �ȸ��½Ƕȣ���ʱһ��ʱ����߼��������Ϊ��ǰ��λ�ã���д��EEPROM�С�
*/
void absEncSetLogicZero(u8 num);

/*
* ��ȡ��ǰ�߼�����λ��
*/
float absEncGetLogicZero(u8 num);

#endif






