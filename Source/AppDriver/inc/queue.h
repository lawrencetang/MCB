/*
2014.04.12
yshy119
*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include "stm32f10x_type.h"

#define QUEUE_MAX 		256

/* 传输队列定义 */
typedef struct _FIFO_Queue
{
    u32 inpos;    /*入口*/
	u32 outpos;    /*入口*/
    u8 buffer[QUEUE_MAX];  /*队列数据*/
}FIFO_Queue; //lt:使用循环队列


/* 初始化队列，清空所有参数 */
void QueueInit(FIFO_Queue* fq);

/* 获取队列长度 */
u16 QueueGetLen(FIFO_Queue* fq);

/*
* 向队列插入一个字符
* 返回值：0 插入失败，1 插入成功
*/
u16 QueueInByte(FIFO_Queue* fq, u8 byte);


/*
* 从队列中取出一个字符
* 返回值：0 取出失败，1 取出成功
*/
u16 QueueOutByte(FIFO_Queue* fq, u8* byte);


/*
* 向队列插入一个字符串
* 返回值：0 插入失败，1 插入成功
*/
u16 QueueIn(FIFO_Queue* fq, u8* buffer, u8 len);

/*
* 从队列中取出一个字符串
* 返回值：0 取出失败，1 取出成功
*/
u16 QueueOut(FIFO_Queue* fq, u8* buffer, u8 len);

#endif
