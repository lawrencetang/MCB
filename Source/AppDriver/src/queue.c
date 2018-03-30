
#include "queue.h"
#include "string.h"


/* 初始化队列，清空所有参数 */
void QueueInit(FIFO_Queue* fq)
{
	fq->inpos = 0;
	fq->outpos = 0;//lt:初始化入口出口地址均为0
	memset(fq->buffer, 0, QUEUE_MAX);//lt:QUEUE_MAX = 256	
}


/* 获取队列长度 */
u16 QueueGetLen(FIFO_Queue* fq)
{
	return (fq->inpos >= fq->outpos) ? (fq->inpos - fq->outpos) : (QUEUE_MAX - (fq->outpos - fq->inpos));
}//lt:环形队列元素个数的算法


/*
* 向队列插入一个字符
* 返回值：0 插入失败，1 插入成功
*/
u16 QueueInByte(FIFO_Queue* fq, u8 byte)
{
	if(QueueGetLen(fq) > (QUEUE_MAX - 1))
		return 0;

	fq->buffer[fq->inpos] = byte;  //lt:将数据插到入口
	fq->inpos = (fq->inpos + 1)%QUEUE_MAX;//lt:保证inpos在0-255

	return 1;		
}


/*
* 从队列中取出一个字符
* 返回值：0 取出失败，1 取出成功
*/
u16 QueueOutByte(FIFO_Queue* fq, u8* byte)
{
	if(QueueGetLen(fq) < 1)
	{
		return 0;
	}
	*byte = fq->buffer[fq->outpos];  //lt:将数据从出口取出
	fq->outpos = (fq->outpos + 1)%QUEUE_MAX;//lt:保证outpos在0-255,走到255后的下一个outpos为0
	return 1;		
}



/*
* 向队列插入一个字符串
* 返回值：插入长度
*/
u16 QueueIn(FIFO_Queue* fq, u8* buffer, u8 len)
{
	u16 q_len = 0;
	u16 i;

	q_len = QueueGetLen(fq);

	if((q_len + len) > (QUEUE_MAX - 1))
		return 0;//lt:如果插入的长度超出队列的最大值，返回0

	for(i = 0; i < len; i++)
	{
		fq->buffer[fq->inpos] = buffer[i];
		fq->inpos = (fq->inpos + 1)%QUEUE_MAX;
	}
	return i;		
}


/*
* 从队列中取出一个字符串
* 返回值：取出长度
*/
u16 QueueOut(FIFO_Queue* fq, u8* buffer, u8 len)
{
	u16 q_len = 0;
	u16 i;

	q_len = QueueGetLen(fq);

	if(q_len < len)
		len = q_len;//lt:队列的长度小于请求长度

	for(i = 0; i < len; i++)
	{
		buffer[i] = fq->buffer[fq->outpos];//lt:将队列数据放入buffer中
		fq->outpos = (fq->outpos + 1)%QUEUE_MAX;
	}
	return i;		
}





