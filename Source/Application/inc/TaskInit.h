#ifndef _TASK_INIT_H
#define _TASK_INIT_H

#include "ucos_ii.h"
#include <includes.h>

#define TAST_BEGIN_WAIT_TICKS	20		// ����������Ϊ�˷�ֹһֱռ��Ӱ����������Ĵ�����������ʱ


#define START_NOT_OVER	0x00
#define START_OVER		0x01
extern volatile u8 g_u8StartOver;



/****�����ջ��С****/
#define TSK_SIZE_START 			512 	  //���������ջ��С
#define TASK_STK_SIZE 			1024	  //���������ջ��С


/****�����ջ****/
extern OS_STK TaskStart_STK[TSK_SIZE_START];
extern OS_STK TASK1_STK[TASK_STK_SIZE];
extern OS_STK TASK2_STK[TASK_STK_SIZE];
extern OS_STK TASK3_STK[TASK_STK_SIZE]; 
extern OS_STK TASK4_STK[TASK_STK_SIZE];
extern OS_STK TASK5_STK[TASK_STK_SIZE];


/**** �������ȼ� ****/
#define TaskStart_PRIO		30
#define TASK1_PRIO			4
#define TASK2_PRIO			5
#define TASK3_PRIO			6
#define TASK4_PRIO			7
#define TASK5_PRIO			8


/**** �ź��� ****/
extern OS_EVENT *g_EventUSBRx; 	//USB����
extern OS_EVENT *g_EventUSBTx; 	//USB����


/**** ������ ****/
void TaskStart(void* pdata); 	//��������
void task1(void* pdata); 		//����1��USB����
void task2(void* pdata); 		//����2��USB����
void task3(void* pdata); 		//����3���������1
void task4(void* pdata); 		//����4���������2 
void task5(void* pdata); 		//����5�����¿���


#endif 
