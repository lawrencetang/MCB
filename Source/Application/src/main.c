/***********************************************************
 * �� �� ����main.c 
 *
 * ��    ��������������
 ***********************************************************/


 
#include "TaskInit.h"
#include "usbDriver.h"
#include "AppDriver.h"
#include "Application.h"


/**
 * ��ʼ�����������Ƿ������ϵͳ������Ĭ��ֵΪSTART_NOT_OVER
 * TaskInit�����������whileǰ��ΪSTART_OVER
 * �����û�������Ҫ�ȵ�״̬��ΪSTART_OVER�󣬲ſ�ʼ����
 */
volatile u8 g_u8StartOver = START_NOT_OVER;

/****�����ջ****/
OS_STK TaskStart_STK[TSK_SIZE_START];
OS_STK TASK1_STK[TASK_STK_SIZE];
OS_STK TASK2_STK[TASK_STK_SIZE];
OS_STK TASK3_STK[TASK_STK_SIZE]; 
OS_STK TASK4_STK[TASK_STK_SIZE];
OS_STK TASK5_STK[TASK_STK_SIZE];//lt:�����������Ķ�ջ�ռ�


/**** �ź��� ****/
OS_EVENT *g_EventUSBRx; 	//USB����
OS_EVENT *g_EventUSBTx; 	//USB����//lt:�����¼����ƿ�



int main (void)
{																    
  OSInit();                          /* Initialize "uC/OS-II, The Real-Time Kernel" */  
	OSTaskCreate(TaskStart, (void*)0, &TaskStart_STK[TSK_SIZE_START - 1], TaskStart_PRIO);
	OSStart();                                       
}



void TaskStart(void* pdata)
{
//	u32 data;
	// �ó�ʼ��δ������־
	g_u8StartOver = START_NOT_OVER;

	BSP_Init();	// �弶��ʼ�� 

	
	/*�����ź���*/
	g_EventUSBRx			= OSSemCreate(0); 
	g_EventUSBTx			= OSSemCreate(0);//lt:���ش������¼����ƿ��ַ��������¼����ƿ�ָ��


	/* ������������ */
	OSTaskCreate(task1, (void*)0, &TASK1_STK[TASK_STK_SIZE-1], TASK1_PRIO);//lt:���ȼ�4,5,6,7,8
	OSTaskCreate(task2, (void*)0, &TASK2_STK[TASK_STK_SIZE-1], TASK2_PRIO);
	OSTaskCreate(task3, (void*)0, &TASK3_STK[TASK_STK_SIZE-1], TASK3_PRIO);
	OSTaskCreate(task4, (void*)0, &TASK4_STK[TASK_STK_SIZE-1], TASK4_PRIO);
	OSTaskCreate(task5, (void*)0, &TASK5_STK[TASK_STK_SIZE-1], TASK5_PRIO);

	OSTimeDlyHMSM(0,0,0,100);

//	flashReadProtect(); 		/* оƬ����������ֹ���򱻶��� */

	OSTimeDlyHMSM(0,0,0,100);

	
	// �ó�ʼ��������־
	g_u8StartOver = START_OVER; 

	while(1)
	{
		powerKeyDet(); 	//ѭ������Դ��
		cswKeyDet(); 	//ѭ�������Ա�״̬

		OSTimeDlyHMSM(0,0,0,100);
		
//		data = deviceIDCheck() | serialIDCheck() | crcCheck();
//		if(!data)
//		{
//			data = 2;
//			while(data--)
//			{
//				OSTimeDly(80);
//				LEDREV();
//			}	
//		}
	} 																
} 

