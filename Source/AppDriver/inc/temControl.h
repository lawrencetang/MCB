/*
temControl.h

*/


#ifndef _TEMPERATURE_APP_H
#define _TEMPERATURE_APP_H

#include "ucos_ii.h"
#include "stm32f10x_gpio.h"
#include "frame.h"
#include "temDriver.h"
#include "pcComm.h"

/* �豸���в��� */
#define GAIN 					6.36  	//�Ŵ���������


#define TEM_TOTAL_TIME 			100		//һ�����ε����ڣ���λ��ms


#define TEM_MAX_TEMP 			100.0 	//Ŀ���¶����ֵ
#define TEM_MIN_TEMP 			10.0 	//Ŀ���¶���Сֵ

/* 
�¶��쳣�ж�
ÿ TEMP_CYCLE_CTRL �� PWM �����һ���¶����
��Ŀ���¶����� TEMP_VALID ���⣬���μ����¶Ȳ������� TEMP_VALID_ERR ���ڣ���Ϊ�¶�û�б仯�����¶��쳣
 */
#define TEM_CYCLE_CTRL 			1200 	//���ȶ��ٸ�PWM����֮���¶�û�б仯����Ϊ���쳣
#define TEM_DIS_DIF				5.0 	//��Ŀ���¶ȶ��ٶ���������ʱ�䲻�䣬�������Ϊ���쳣
#define TEM_VALID_ERR 			1.0 	//��Ŀ���¶Ƚ�Զʱ��ÿ�μ�������¶ȱ仯ֵ

#define TEM_PID_VALID 			0.2  	//��Ŀ���¶�һ����Χ�ڲ�����PID�㷨������PID�㷨��Ч

#define KP 	2.5
#define KI  0.05
#define KD  0.01

/*���¿��ƽṹ��*/
typedef struct _TEM_CTRL
{
	/* ���ڼ�¼�Ƿ������¿��ƣ�����¼��ǰ��״̬ */
	u8 start;    /* ��ͣ��0 ��ʾ�رպ��£�1 ��ʾ�������� */
	u8 status;   /* ״̬��0 ��ʾδ���к���״̬��1 ��ʾ�ѽ������״̬����δ��������ʱ����û������ */

	/* ���ڼ�¼�������º�ĸ��ֲ���������PID�㷨����δ��������ʱ����û������ */
	float dt;   /* Ŀ���¶� */
	float e0;   /* Ŀ���¶��뵱ǰ�¶ȵĲ� */
	float e1;   /* �ϴο���ʱĿ���¶����ϴ��¶ȵĲ� */
	float es;   /* ������ */

	u8 ad_channel; /* ����ϵͳ��Ӧ��ADͨ���ţ�����ڳ�ʼ��ʱȷ����֮������Ķ� */
	OS_EVENT *Event; 	  	
}TEM_CTRL;

extern TEM_CTRL g_tem[TEM_NUM];


/* ���¿���Ӧ�ò��ʼ�� */
u8 temAppInit(void);

/*
��������¶ȿ��Ʋ���
*/
u8 temAppClear(u8 p_id);

/* 
* ��ȡ��ǰ�¶�
* ����ͨ�������ʵ���¶�ֵ��ʵ���¶�ֵ���ɼ���������¶��ټ������ 
*/
float temAppGetTem(u8 p_id);

/*
* ����Ŀ���¶��뵱ǰ�¶ȼ����������ʱ��
* ��ʱ��Ϊ��TEM_TOTAL_TIME
* ��Ŀ���¶ȴ��ڵ�ǰ�¶�ʱ��������������Ŀ���¶�С�ڵ�ǰ�¶�ʱ�����ظ���
*/
s16 temGetHighTime(u8 p_id);


/*
* �����������
* ��ÿ TEM_TOTAL_TIME ʱ���ڽ��бջ����ƣ���ͨ��ʱ����PID�㷨����ó� 
*/
u8 temHeatCool(u8 p_id);


/*
* �¿�ָ������
* ������λ���·����¶ȿ���ָ��
*/
u8 temCMDDeal(pcFrame frame);


/*
* ���¿���������
*/
void temTaskDeal(u8 p_id);

#endif

