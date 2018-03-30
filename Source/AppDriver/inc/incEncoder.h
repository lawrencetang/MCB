

#ifndef __INC_ENC_H
#define __INC_ENC_H

#include "step_motor.h"
#include "my_protocal.h"
#include "header.h"

#define ENC_PULSE360 		2000.0 /* ������һȦ�����������������źŸ�1000�� */

#define ENC_DegreeToPulse(d) 	(ENC_PULSE360*d/360.0) /* �ɽǶ�ת���ɱ����������� */
#define ENC_PulseToDegree(p) 	(360.0*p/ENC_PULSE360) /* �ɱ����������廻�ɳɽǶ� */

/*�����������ṹ��*/
typedef struct _INC_ENC
{
	u16 cnt;		 //��ʱ������ֵ
	s32 pulse;		 //�������������
}INC_ENC;
extern INC_ENC g_enc[SM_No];

/*��ʼ��������*/
void ENC_InitENC(u8 num);

/*���±���������õ�ǰλ��*/
u8 ENC_UpdatePos(u8 num);

/*���ɼ����ı�������������ת���ɽǶ�*/
u32 ENC_PulseToAngle(u32 pulse);

/*Timer1��ʼ��*/
void Timer1Init(void);

/*Timer8��ʼ��*/
void Timer8Init(void);

#endif



