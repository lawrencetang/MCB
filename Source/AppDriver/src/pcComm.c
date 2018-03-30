

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

	len = QueueOut((FIFO_Queue*) &g_PCTxQueue, (u8 *)&l_buff, pcTxQueueLen());//lt:�Ӷ�����ȡ���ַ���,ȡ�����е�ȫ������
	
	UserToPMABufferCopy(l_buff, ENDP1_TXADDR, len);//lt:����usb�˵�1�ķ��ͻ�����,�����Ͷ����е����ݴ���64��ô�죿
	SetEPTxCount(ENDP1, len);
	SetEPTxValid(ENDP1);//lt:usb��ʼ��ʱ�˵�1������NAK״̬����Ӧ���˵�1��״̬��ΪValid
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
��PC���Ĵ��ڷ��Ͷ�����д��һ��ԭʼ����֡������֡�������롢��֡�ȴ���֮��֪ͨ���ڷ���������з��͡�
*/
void pcTxQueueAddFrame(pcFrame *frame)
{
	u8 l_res = 0;
	OS_CPU_SR  cpu_sr = 0;	
	
	framePackage(frame);  //lt:���ݴ����д���ֽ�ͷ�����������ֽڣ�У���ֽ�

	while(!l_res)
	{
		//�����ȣ���������Ķ���Ҫ����
		OS_ENTER_CRITICAL(); 
		l_res = QueueIn((FIFO_Queue *)&g_PCTxQueue, (u8 *)frame, FRAME_LEN);//lt:���Ͷ��в����ַ��������ط��͵��ַ������ȸ�l_res
		OS_EXIT_CRITICAL();//lt:�����ֹ�ж���Ϊ�˱�������ַ����뵽�м�ʱ�ں˱�����������ռ��ʹ�����е����ݳ��ֻ��ң���Ҫ��

		if(!l_res)  //lt:����ʣ��ռ䲻����l_res����0�������ж�
		{
			OSSemPost(g_EventUSBTx); //lt:post�źţ�ȡ�ȴ�������е�HPT����
			OSTimeDly(100);
		}//lt:ֱ���������㹻�ռ������whileѭ������֪ͨ����ϵͳ��Ҫ��������
	}
	OSSemPost(g_EventUSBTx);//lt:post�źţ�ȡ�ȴ�������е�HPT����
}


u32 g_pcRxTimeOut = 0;

/*
* ����ն����в���һ������
* ����ֵ��0 ����ʧ�ܣ�1 ����ɹ�
*/
u16 pcRxQueueIn(u8* buffer, u8 buflen)
{
	u16 l_ret;
	u32 ticks = OSTimeGet();
	if((ticks - g_pcRxTimeOut) > 100)  /* ��������ж�ʱ�䳬��100ms����ֱ����ս��ն��� */
	{
		g_PCRxQueue.inpos = 0;
		g_PCRxQueue.outpos = 0;
	}
	g_pcRxTimeOut = ticks;
	l_ret = QueueIn((FIFO_Queue *)&g_PCRxQueue, buffer, buflen);//lt:���ز���ĳ���
	if(pcRxQueueLen())
	{
		OSSemPost(g_EventUSBRx);//lt:�ܹ���usb���������͵�buffer��ȡ�����ݷ�����У���ճ��g_ecentusbrx�źţ��Ӷ��ý�����λ�����ݵ����������ź�ʱ����ź�
	}
	return l_ret;
}


/*
 * ��ȡ֡���ӽ��ն�������ȡ��һ��֡��Ҫ��У��Ҳ��ȷ
 * ����ֵ��0 ����ȡ����1 ʧ�ܡ�
*/
u8 pcRxQueueOutFrame(pcFrame *frame)
{
	u16 temlen = 0;
	u8 data;
	u8 tembuf[FRAME_LEN];	

	if(pcRxQueueLen() < FRAME_LEN)
		return 1;  //lt:���ն������ֽ���С��12

	while(pcRxQueueLen())  //lt:���ն������ֽڳ���
	{
		QueueOutByte((FIFO_Queue *)&g_PCRxQueue, (u8*) &data);
		if(temlen == 0)
		{
			if(data == 0xff)
			{
				tembuf[temlen++] = data;
			}//lt:������ݲ�Ϊ0xff��temlen�����Լ�
		}
		else if(temlen == 1)
		{
			if(data != 0xff)
			{
				tembuf[temlen++] = 0xff;//lt:ǿ��0xff
			}
			tembuf[temlen++] = data;			
		}
		else
		{
			tembuf[temlen++] = data;  //lt:�����ݽ���tembuf
			if(temlen >= FRAME_LEN)
			{
				if(frameCheck((pcFrame*)tembuf) == 0) //lt:���һ���ֽ�ΪУ���ֽڣ�У����ȷΪ0
				{
					memcpy((u8*)frame, (u8*)&tembuf[0], FRAME_LEN); //lt:Ŀ�ĵ�ַframe
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



