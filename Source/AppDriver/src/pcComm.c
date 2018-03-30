

#include "pcComm.h"
#include "os_cpu.h"
#include "protocal.h"
#include "string.h"

#include "usbDriver.h"
#include "Application.h"

FIFO_Queue g_PCTxQueue;
FIFO_Queue g_PCRxQueue;

void pcQueueInit(void)
{
	g_PCTxQueue.inpos = 0;
	g_PCTxQueue.outpos = 0;

	g_PCRxQueue.inpos = 0;
	g_PCRxQueue.outpos = 0;
}

u16 pcTxQueueLen(void)
{
	return QueueGetLen((FIFO_Queue*) &g_PCTxQueue);
}

u16 pcRxQueueLen(void)
{
	return QueueGetLen((FIFO_Queue*) &g_PCRxQueue);
}

void pcSendTxQueue(void)
{
	u8 l_buff[QUEUE_MAX];
	u16 len;

	len = QueueOut((FIFO_Queue*) &g_PCTxQueue, (u8 *)&l_buff, pcTxQueueLen());//lt:从队列中取出字符串,取出队列的全部长度
	
	UserToPMABufferCopy(l_buff, ENDP1_TXADDR, len);//lt:放入usb端点1的发送缓冲区,若发送队列中的数据大于64怎么办？
	SetEPTxCount(ENDP1, len);
	SetEPTxValid(ENDP1);//lt:usb初始化时端点1发送是NAK状态，故应将端点1的状态变为Valid
}



void pcSendStr(u8* str, u8 len)
{
	UserToPMABufferCopy(str, ENDP1_TXADDR, len);
	SetEPTxCount(ENDP1, len);
	SetEPTxValid(ENDP1);
}



void pcSendByte(u8 ch)
{
	UserToPMABufferCopy((u8*) &ch, ENDP1_TXADDR, 1);
	SetEPTxCount(ENDP1, 1);
	SetEPTxValid(ENDP1);
}


/*
向PC机的串口发送队列里写入一个原始数据帧，数据帧经过编码、组帧等处理之后，通知串口发送任务进行发送。
*/
void pcTxQueueAddFrame(pcFrame *frame)
{
	u8 l_res = 0;
	OS_CPU_SR  cpu_sr = 0;	
	
	framePackage(frame);  //lt:数据打包，写两字节头，上行命令字节，校验字节

	while(!l_res)
	{
		//不调度，本处代码改动需要谨慎
		OS_ENTER_CRITICAL(); 
		l_res = QueueIn((FIFO_Queue *)&g_PCTxQueue, (u8 *)frame, FRAME_LEN);//lt:向发送队列插入字符串，返回发送的字符串长度给l_res
		OS_EXIT_CRITICAL();//lt:这里禁止中断是为了避免插入字符插入到中间时内核被下行任务抢占掉使队列中的数据出现混乱（重要）

		if(!l_res)  //lt:队列剩余空间不够，l_res返回0，进入判断
		{
			OSSemPost(g_EventUSBTx); //lt:post信号，取等待任务表中的HPT运行
			OSTimeDly(100);
		}//lt:直到队列有足够空间才跳出while循环，在通知操作系统需要发送数据
	}
	OSSemPost(g_EventUSBTx);//lt:post信号，取等待任务表中的HPT运行
}


u32 g_pcRxTimeOut = 0;

/*
* 向接收队列中插入一串数据
* 返回值：0 插入失败，1 插入成功
*/
u16 pcRxQueueIn(u8* buffer, u8 buflen)
{
	u16 l_ret;
	u32 ticks = OSTimeGet();
	if((ticks - g_pcRxTimeOut) > 100)  /* 如果两次中断时间超过100ms，则直接清空接收队列 */
	{
		g_PCRxQueue.inpos = 0;
		g_PCRxQueue.outpos = 0;
	}
	g_pcRxTimeOut = ticks;
	l_ret = QueueIn((FIFO_Queue *)&g_PCRxQueue, buffer, buflen);//lt:返回插入的长度
	if(pcRxQueueLen())
	{
		OSSemPost(g_EventUSBRx);//lt:能够从usb缓冲区发送到buffer中取出数据放入队列，则粘贴g_ecentusbrx信号，从而让接收上位机数据的任务请求信号时获得信号
	}
	return l_ret;
}


/*
 * 提取帧：从接收队列中提取出一个帧，要求校验也正确
 * 返回值：0 正常取出，1 失败。
*/
u8 pcRxQueueOutFrame(pcFrame *frame)
{
	u16 temlen = 0;
	u8 data;
	u8 tembuf[FRAME_LEN];	

	if(pcRxQueueLen() < FRAME_LEN)
		return 1;  //lt:接收队列中字节数小于12

	while(pcRxQueueLen())  //lt:接收队列中字节长度
	{
		QueueOutByte((FIFO_Queue *)&g_PCRxQueue, (u8*) &data);
		if(temlen == 0)
		{
			if(data == 0xff)
			{
				tembuf[temlen++] = data;
			}//lt:如果数据不为0xff则temlen不会自加
		}
		else if(temlen == 1)
		{
			if(data != 0xff)
			{
				tembuf[temlen++] = 0xff;//lt:强制0xff
			}
			tembuf[temlen++] = data;			
		}
		else
		{
			tembuf[temlen++] = data;  //lt:放数据进入tembuf
			if(temlen >= FRAME_LEN)
			{
				if(frameCheck((pcFrame*)tembuf) == 0) //lt:最后一个字节为校验字节，校验正确为0
				{
					memcpy((u8*)frame, (u8*)&tembuf[0], FRAME_LEN); //lt:目的地址frame
					return 0;
				}
				else
				{
					temlen = 0;
				}
			}
		} 
	}
	return 1; 
}



