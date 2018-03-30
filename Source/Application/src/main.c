/***********************************************************
 * 文 件 名：main.c 
 *
 * 描    述：程序主函数
 ***********************************************************/


 
#include "TaskInit.h"
#include "usbDriver.h"
#include "AppDriver.h"
#include "Application.h"


/**
 * 初始化启动过程是否结束，系统启动后默认值为START_NOT_OVER
 * TaskInit任务进入最后的while前置为START_OVER
 * 所有用户任务，需要等到状态变为START_OVER后，才开始工作
 */
volatile u8 g_u8StartOver = START_NOT_OVER;

/****任务堆栈****/
OS_STK TaskStart_STK[TSK_SIZE_START];
OS_STK TASK1_STK[TASK_STK_SIZE];
OS_STK TASK2_STK[TASK_STK_SIZE];
OS_STK TASK3_STK[TASK_STK_SIZE]; 
OS_STK TASK4_STK[TASK_STK_SIZE];
OS_STK TASK5_STK[TASK_STK_SIZE];//lt:定义各个任务的堆栈空间


/**** 信号量 ****/
OS_EVENT *g_EventUSBRx; 	//USB接收
OS_EVENT *g_EventUSBTx; 	//USB发送//lt:创建事件控制块



int main (void)
{																    
  OSInit();                          /* Initialize "uC/OS-II, The Real-Time Kernel" */  
	OSTaskCreate(TaskStart, (void*)0, &TaskStart_STK[TSK_SIZE_START - 1], TaskStart_PRIO);
	OSStart();                                       
}



void TaskStart(void* pdata)
{
//	u32 data;
	// 置初始化未结束标志
	g_u8StartOver = START_NOT_OVER;

	BSP_Init();	// 板级初始化 

	
	/*创建信号量*/
	g_EventUSBRx			= OSSemCreate(0); 
	g_EventUSBTx			= OSSemCreate(0);//lt:返回创建的事件控制块地址给定义的事件控制块指针


	/* 创建其他任务 */
	OSTaskCreate(task1, (void*)0, &TASK1_STK[TASK_STK_SIZE-1], TASK1_PRIO);//lt:优先级4,5,6,7,8
	OSTaskCreate(task2, (void*)0, &TASK2_STK[TASK_STK_SIZE-1], TASK2_PRIO);
	OSTaskCreate(task3, (void*)0, &TASK3_STK[TASK_STK_SIZE-1], TASK3_PRIO);
	OSTaskCreate(task4, (void*)0, &TASK4_STK[TASK_STK_SIZE-1], TASK4_PRIO);
	OSTaskCreate(task5, (void*)0, &TASK5_STK[TASK_STK_SIZE-1], TASK5_PRIO);

	OSTimeDlyHMSM(0,0,0,100);

//	flashReadProtect(); 		/* 芯片读保护，防止程序被读出 */

	OSTimeDlyHMSM(0,0,0,100);

	
	// 置初始化结束标志
	g_u8StartOver = START_OVER; 

	while(1)
	{
		powerKeyDet(); 	//循环检测电源键
		cswKeyDet(); 	//循环检测测试臂状态

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

