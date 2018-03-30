/*
2012.01.15
*/

#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include "ucos_ii.h"
#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"
#include "frame.h"
#include "timer3.h"

#define SM_NUM 				2  		/* ������������� */

#define SM_STS_IDLE 		0x00	/* ����������� */
#define SM_STS_BUZY 		0x01	/* ���������æ */
#define SM_STS_FAULT 		0x02	/* ����������� */
#define SM_STS_POWON 		0x03	/* ����������ϵ� */

/* �������1�õ���PE13~ED15 */
#define SM1_PIN_EN 			GPIO_Pin_13	  	/*PE13��ʹ�ܿ������ţ�0 ʹ��   1 ��ʹ��*/
#define SM1_PIN_STP			GPIO_Pin_14	  	/*PE14��ת����������*/
#define SM1_PIN_DIR			GPIO_Pin_15	  	/*PE15������������ţ�0 ��ת   1 ��ת*/	

/* �������2�õ���PE8~PE10 */
#define SM2_PIN_EN 			GPIO_Pin_8	  	/*PE8��ʹ�ܿ������ţ�0 ʹ��   1 ��ʹ��*/
#define SM2_PIN_STP			GPIO_Pin_9	  	/*PDE9��ת���������� */
#define SM2_PIN_DIR			GPIO_Pin_10	  	/*PE10������������ţ�0 ��ת   1 ��ת*/

/*ltj �ӽ������ź�*/
#define SM_ZS           GPIO_pin_6
#define SM_RS           GPIO_pin_14


#define SM_SETEN(num) 	GPIOE->ODR ^= g_sm[num].pin.en
#define SM_CLREN(num) 	GPIOE->ODR &= ~g_sm[num].pin.en

#define SM_REVSTP(num) 	GPIOE->ODR ^= g_sm[num].pin.stp
#define SM_CLRSTP(num) 	GPIOE->ODR &= ~g_sm[num].pin.stp

#define SM_SETDIR(num) 	g_sm[num].dir?(GPIOE->ODR |= g_sm[num].pin.dir):(GPIOE->ODR &= ~g_sm[num].pin.dir)

#define SM_STEP360   		3200.0      	/*�������ת360������Ҫ�Ĳ���������ϸ��������*///lt:�����1.8�㣬16ϸ��	

#define SM_MAX_ANGLE 		360

#define SM_ACC_TICKS 		0.005
#define SM_MAX_WAIT_TICKS 	10
#define SM_MIN_WAIT_TICKS 	2

#define SM_EQUAL_ANGLE_ERROR 	1.0 		/* �����Ƕ���ȵ��������������������Ϊ���� */



/*����������ת������*/
#define SM_DegreeToPulse(d) 			((int)(SM_STEP360*d/360+0.5))  /* ����ת��Ϊ��Ӧ�ĵ��ת������ */ //lt:3200/360*d
#define SM_PulseToDegree(p) 			((int)(360*p/SM_STEP360+0.5))  /* ���ת������ת��Ϊ��Ӧ�Ķ��� */  //lt:360/3200*p



enum {SM1 = 0, SM2 = 1};




/*
* �������������ֹͣ���ٶȶ�������ת���������ٶȽϿ졣
* ÿ�ζԵ�����в�������������Ҫ�������ٺͼ������������̡�
* Ϊ�˷�����ƣ���ʱ��ֻ���𵥸����̵Ŀ��ƣ����������ת����
* ��ˣ������˵���ת�����̲�����������һ�β���������
*
* ����ת�����̲�������������ת��û�нǶȸ��ֻ����������ʱ��ticks����
* 						1 sts��ȷ���Ƿ����ת��
* 						2 totalPulse������ת����������
* 						3 remainPulse��ʣ��������
* 						4 spdTicks��ÿ����������Ҫ��ticks�������������ٶ�
* 						5 acc��ÿ�������spdTicks�ĸı����������ڿ��Ƽ��١����١�����
* 						6 ticks�����ڶ�ʱ���жϵļ���
*
* һ��������ת�����Ʋ�������������ת����ͨ���Ƕ������Ƶģ�
* 						1 flag��ת����־������ָʾ�豸״̬����λ������1������ת����ת����ɺ���0������豸�������޷�����ת��
* 						2 goalAgnle��Ŀ��Ƕȣ���λ����Ƶ�Ŀ��Ƕȣ�������ת����Ŀ��Ƕȡ�
* 						3 realAngle����ǰ�Ƕȣ�ָʾ�����ǰ�Ƕȡ�
*/



/* �������IO������ƽṹ�� */
typedef struct _SM_IO_PIN
{
	u16 en;
	u16 stp;
	u16 dir;
}SM_IO_PIN;


/* �������һ��ת�����̿��Ʋ��� */
typedef struct _SM_ROTATE
{
	u8 sts; 			/* ����ת��״ָ̬ʾ��0 ת��ֹͣ�� 1 ����ת�� */
	u32 totalPulse; 	/* ����ת�������� */
	u32 remainPulse; 	/* ����ת��ʣ�������� */
	float spdTicks; 	/* STP�����ʱ�䣬����ʱ���жϴ��������ڿ���ת���ٶ� */	
	float acc; 			/* ת�������п���spdTicks�仯�Ĳ���������ʱΪ��������ʱΪ��������ʱΪ0 */ 
	s16 ticks; 			/* ����ʵʱ��������м��� */	
}SM_ROTATE;


/* �������ȫ�ֲ����������� */
typedef struct _SM_PARA
{
	float accTicks;
	float maxTicks;
	float minTicks;
	OS_EVENT *event;
}SM_PARA;



/*����������Ʋ����ṹ��*/
typedef struct _STEP_MOTOR
{
	u8 flag; 	 			/* ���״̬��0 ���У� 1 ��ת�� 2 ���ϣ� 3 δ��ʼ�� */
	u8 dir;
	float goalAngle;		/* Ҫת������Ŀ��Ƕȣ�����λ������ */
	float realAngle;		/* ��ǰʵ�ʽǶ� */
	SM_IO_PIN pin; 			/* ���������Ӧ�Ŀ������� */
	SM_ROTATE ro;			/* �������ת������ */
	SM_PARA para; 			/* �������ȫ�ֲ��� */
}STEP_MOTOR;


extern STEP_MOTOR g_sm[SM_NUM];

/*
���������ʼ�����������źͲ���
*/
void smInit(void);



/*
*/
u8 smSetCMD(pcFrame frame);



/*
���������������е���ת����صĲ���
*/
void smClearRotate(u8 num);


/*
�������ת��
*/
void smStartSM(u8 num, u32 pulse, float acc);



/*
ֹͣ���ת��
*/
void smStopSM(u8 num);


/*
����������Ƶ�tick
������ͨ����ʱ�����ã�Ҫ��ʱ����ʱ������20us����ÿ20us����뱾����һ�Ρ�
ÿ�ο��ƶ���Ҫ�������ٺͼ��ٹ���
*/
void smRotateTick(void);


/*
ת������
*/
u8 smRotate(u8 num);



/*
��������Լ�
�õ��ת��10�ȣ���ת��ԭ����λ�ã��ж�ת���Ƿ���������������򷵻سɹ������򷵻�ʧ�ܡ�
*/
u8 smCheck(u8 num);




/*
* ���²�������ĽǶ�
*/
void smUpdateAngle(u8 num);


/*
�ж������Ƕ��Ƿ����
��������ǶȲ�С��SM_EQUAL_ANGLE_ERROR������ȣ������ж�Ϊ����
����ֵ��1 ���ȣ�0 ���
*/
u8 smJudgeEqualAngle(float a1, float a2);

#endif



