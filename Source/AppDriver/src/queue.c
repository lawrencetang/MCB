
#include "queue.h"
#include "string.h"


/* ��ʼ�����У�������в��� */
void QueueInit(FIFO_Queue* fq)
{
	fq->inpos = 0;
	fq->outpos = 0;//lt:��ʼ����ڳ��ڵ�ַ��Ϊ0
	memset(fq->buffer, 0, QUEUE_MAX);//lt:QUEUE_MAX = 256	
}


/* ��ȡ���г��� */
u16 QueueGetLen(FIFO_Queue* fq)
{
	return (fq->inpos >= fq->outpos) ? (fq->inpos - fq->outpos) : (QUEUE_MAX - (fq->outpos - fq->inpos));
}//lt:���ζ���Ԫ�ظ������㷨


/*
* ����в���һ���ַ�
* ����ֵ��0 ����ʧ�ܣ�1 ����ɹ�
*/
u16 QueueInByte(FIFO_Queue* fq, u8 byte)
{
	if(QueueGetLen(fq) > (QUEUE_MAX - 1))
		return 0;

	fq->buffer[fq->inpos] = byte;  //lt:�����ݲ嵽���
	fq->inpos = (fq->inpos + 1)%QUEUE_MAX;//lt:��֤inpos��0-255

	return 1;		
}


/*
* �Ӷ�����ȡ��һ���ַ�
* ����ֵ��0 ȡ��ʧ�ܣ�1 ȡ���ɹ�
*/
u16 QueueOutByte(FIFO_Queue* fq, u8* byte)
{
	if(QueueGetLen(fq) < 1)
	{
		return 0;
	}
	*byte = fq->buffer[fq->outpos];  //lt:�����ݴӳ���ȡ��
	fq->outpos = (fq->outpos + 1)%QUEUE_MAX;//lt:��֤outpos��0-255,�ߵ�255�����һ��outposΪ0
	return 1;		
}



/*
* ����в���һ���ַ���
* ����ֵ�����볤��
*/
u16 QueueIn(FIFO_Queue* fq, u8* buffer, u8 len)
{
	u16 q_len = 0;
	u16 i;

	q_len = QueueGetLen(fq);

	if((q_len + len) > (QUEUE_MAX - 1))
		return 0;//lt:�������ĳ��ȳ������е����ֵ������0

	for(i = 0; i < len; i++)
	{
		fq->buffer[fq->inpos] = buffer[i];
		fq->inpos = (fq->inpos + 1)%QUEUE_MAX;
	}
	return i;		
}


/*
* �Ӷ�����ȡ��һ���ַ���
* ����ֵ��ȡ������
*/
u16 QueueOut(FIFO_Queue* fq, u8* buffer, u8 len)
{
	u16 q_len = 0;
	u16 i;

	q_len = QueueGetLen(fq);

	if(q_len < len)
		len = q_len;//lt:���еĳ���С�����󳤶�

	for(i = 0; i < len; i++)
	{
		buffer[i] = fq->buffer[fq->outpos];//lt:���������ݷ���buffer��
		fq->outpos = (fq->outpos + 1)%QUEUE_MAX;
	}
	return i;		
}





