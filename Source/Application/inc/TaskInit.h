#ifndef _TASK_INIT_H
#define _TASK_INIT_H

#include "ucos_ii.h"
#include <includes.h>

#define TAST_BEGIN_WAIT_TICKS	20		// 任务启动后，为了防止一直占用影响其他任务的创建，需先延时


#define START_NOT_OVER	0x00
#define START_OVER		0x01
extern volatile u8 g_u8StartOver;



/****定义堆栈大小****/
#define TSK_SIZE_START 			512 	  //启动任务堆栈大小
#define TASK_STK_SIZE 			1024	  //其他任务堆栈大小


/****任务堆栈****/
extern OS_STK TaskStart_STK[TSK_SIZE_START];
extern OS_STK TASK1_STK[TASK_STK_SIZE];
extern OS_STK TASK2_STK[TASK_STK_SIZE];
extern OS_STK TASK3_STK[TASK_STK_SIZE]; 
extern OS_STK TASK4_STK[TASK_STK_SIZE];
extern OS_STK TASK5_STK[TASK_STK_SIZE];


/**** 任务优先级 ****/
#define TaskStart_PRIO		30
#define TASK1_PRIO			4
#define TASK2_PRIO			5
#define TASK3_PRIO			6
#define TASK4_PRIO			7
#define TASK5_PRIO			8


/**** 信号量 ****/
extern OS_EVENT *g_EventUSBRx; 	//USB接收
extern OS_EVENT *g_EventUSBTx; 	//USB发送


/**** 任务函数 ****/
void TaskStart(void* pdata); 	//启动任务
void task1(void* pdata); 		//任务1，USB接收
void task2(void* pdata); 		//任务2，USB发送
void task3(void* pdata); 		//任务3，步进电机1
void task4(void* pdata); 		//任务4，步进电机2 
void task5(void* pdata); 		//任务5，恒温控制


#endif 
