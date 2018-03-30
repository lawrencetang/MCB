/*************************************
所有任务和应用层相关的均在本文件中

yshy119
2014年3月31日
*************************************/


#include "TaskInit.h"

#include "stm32f10xDriver.h"
#include "usbDriver.h"
#include "AppDriver.h"

#include "task.h"

#include "os_cpu.h"





/*
下行响应任务，即处理上位机发下来的指令。
*/
void task1(void* pdata)
{
	u8 err = 0;
	pcFrame frame;

	// 等待初始化结束
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);//lt:退出就绪态20个时钟节拍
	}//lt:未创建完成所用任务，任务均运行于该循环中


	while(1)
	{
		if(pcRxQueueOutFrame((pcFrame *)&frame))
		{
			OSSemPend(g_EventUSBRx, 1000, &err);//lt:将当前任务就绪状态取消，写入等待任务表
			continue ;//lt:有其他任务post该信号才能该任务才能进行接收判断
		}//lt:未接收或者不正确接收进入判断

//		LEDREV();

		if(frame.flag != DOWN_FRAME)/*判断下行帧命令*/
		{
			continue ;
		}  //lt:结束本次循环，进行下次循环

		switch(frame.cmd & 0xf0)  //lt:判断命令种类，保留高4位
		{
		case COM_TEST:  //lt:0x00 测试命令
			CMDTestDeal(frame);
			break;

		case SM_CMD: /* 步进电机控制指令 */  //lt:0x80
			frame.sts = smSetCMD(frame);
			if(frame.sts)  //lt:若指令失败执行，回传数据
			{
				pcTxQueueAddFrame((pcFrame *)&frame);
			}
			break;

		case DCM_CMD:  //lt:0x90 直流电机指令
			frame.sts = dcmSetCMD(frame);
			pcTxQueueAddFrame((pcFrame *)&frame);			
			break;

		case TEM_CMD:  //lt:0xA0 温控指令
			frame.sts = temCMDDeal(frame);
			if(frame.sts)
			{
				pcTxQueueAddFrame((pcFrame *)&frame);
			}
			break;
			
		case BOARD_STATUS:  //lt:0x70 查询指令
			boardStatusCMD(frame);
			break;

		case ENCRYPT_CMD:  //lt:0xD0 加密设备指令
			encryptCMDDeal(frame);
			break;

		default:
			frame.sts = STS_CMD_ERROR;	//lt:以上指令均不是，返回0x03状态指令错误	
			pcTxQueueAddFrame((pcFrame *)&frame);
			break;
		}
		
		memset((u8*)&frame, 0, FRAME_LEN); 
	}
}

/*************************************************
****************  通信发送任务  ******************
********************  SM1  **********************/
void task2(void* pdata)
{
	u8 err = 0;

	// 等待初始化结束
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);
	}


	while(1)
	{
		OSSemPend(g_EventUSBTx, 1000, &err);//lt:将当前任务就绪状态取消，写入等待任务表

		LEDREV();//lt:队列空的时候，不在等待就在等待的路上

		while(pcTxQueueLen())
		{
			LEDREV();
			pcSendTxQueue();//lt:若连发两条下行指令，上行任务在取出队列之后，发送至上位机之前被剥夺，-->
			//lt:-->事件控制块中计数值为2，将第二条下行指令的回复发送完成后计数值为1，故最后还要空发一次上行任务才能进入等待状态。
		}//lt:队列有数据就要发送到usb缓冲区
	}
}




/*************************************************
*****************  步进电机1  ********************
********************  SM1  **********************/
void task3(void* pdata)
{ 
	u8 err;	
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* 等待初始化结束 */
	}

	Timer3Start();

	while(1)
	{
		OSSemPend(g_sm[SM1].para.event, 500, &err);//lt:有信号post后err=0		
		smUpdateAngle(SM1);
		if(0 == err)
		{
			smRotate(SM1);//lt:有信号post后调用该函数		
		}		
	}
}

/*************************************************
*****************  步进电机2  ********************
********************  SM2  **********************/
void task4(void* pdata)
{ 
	u8 err;
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* 等待初始化结束 */
	}

	while(1)
	{
		OSSemPend(g_sm[SM2].para.event, 500, &err);		
		smUpdateAngle(SM2);
		if(0 == err)
		{
			smRotate(SM2);				
		}		
	}
}


/*************************************************
*****************  恒温任务  *********************
********************  TEM  **********************/
void task5(void* pdata)
{ 
	u8 err;
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* 等待初始化结束 */
	}
	while(1)
	{
		OSSemPend(g_tem[TEM1].Event, 500, &err);
		
		temAppGetTem(TEM1);		

		if(0 == err)
		{
			temTaskDeal(TEM1);	
		}		
	}
}

