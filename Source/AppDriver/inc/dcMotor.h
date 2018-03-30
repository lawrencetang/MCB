/*
dcMotor.h
��TIMER5��Ϊֱ�������PWM�������
*/

#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"
#include "frame.h"



#define DCM_NUM 				2  		/* ֱ����������� */


#define DCM_CNT_NUM 			10000	/* ֱ�����PWM���ڣ���λΪ1����ʱtick��һ�����ö�ʱ������Ϊ1us */


#define DCM_RATE_NUM 			3 	  	/* ֱ������ٶȵȼ����� */
/* ֱ�����PWM���ռ�ձ� */
#define DCM_RATE1 				1	  	/* �ٷ�֮ */
#define DCM_RATE2 				10
#define DCM_RATE3 				20


#define DCM1_PIN_OUT 	GPIO_Pin_2 		/* PA2��TIM5_CH3  ֱ�����1������� */
#define DCM2_PIN_OUT 	GPIO_Pin_3 		/* PA3��TIM5_CH4  ֱ�����2������� */

#define TCHSW_PIN_IN 	GPIO_Pin_1 		/* PB1��ֱ�����2�Ĵ������� */


#define DCM1_ON 	GPIO_SetBits(GPIOA, DCM1_PIN_OUT)
#define DCM1_OFF 	GPIO_ResetBits(GPIOA, DCM1_PIN_OUT)

#define DCM2_ON 	GPIO_SetBits(GPIOA, DCM2_PIN_OUT)
#define DCM2_OFF 	GPIO_ResetBits(GPIOA, DCM2_PIN_OUT)

enum {DCM1 = 0, DCM2 = 1};


/* �������IO������ƽṹ�� */
typedef struct _DCMotor
{
	u8 sts; 	/* ֱ�����״̬��0 ֹͣ��1 ���� */
	u8 rate; 	/* ֱ�����PWM�����ռ�ձ� 0 ~ 100 ֮���һ����ֵ */
}DCMotor;

extern DCMotor g_dcm[DCM_NUM];



/*
* ֱ�������ʼ��
*/
void dcmInit(void);

/*
* ֱ������������
* ������λ����������ָ�����ֱ�������
*/
u8 dcmSetCMD(pcFrame frame);

/*
* ����ֱ�����
* ����PWM��������Ѷ�ʱ����Ӧͨ����PWM�Ƚ�ֵ����Ϊ��Ӧ��ռ�ձȲ���
*/
void dcmStartDCM(u8 num);

/*
* ֱֹͣ�����
* ֹͣPWM���������ʱ����Ӧͨ����PWM�Ƚ�ֵ����Ϊ0����һֱ����͵�ƽ
*/
void dcmStopDCM(u8 num);

/*
* �ᴥ���س�ʼ��
* �ᴥ��������ֱ�����2���ֶ����ƣ�ÿ��һ��ֱ�����2״̬�͸ı�
* ��ʼ�������������Ϊ�ⲿ�жϽ�
*/
void tchSWInit(void);

/*
* �ᴥ�����жϷ�����
* ���ڿ���ֱ�����2����ͣ��ÿ��һ��״̬��ת
*/
void tchSWIsr(void);

#endif



