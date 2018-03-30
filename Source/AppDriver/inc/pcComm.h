
/*
20140412
yshy119
*/


#ifndef _MY_PC_UART_H
#define _MY_PC_UART_H

#include "queue.h"
#include "frame.h"



extern FIFO_Queue g_PCTxQueue;
extern FIFO_Queue g_PCRxQueue;

void pcQueueInit(void);
u16 pcTxQueueLen(void); 
u16 pcRxQueueLen(void);
void pcSendTxQueue(void);
void pcSendByte(u8 ch);
void pcSendStr(u8* str, u8 len);

/*
向PC机的串口发送队列里写入一个原始数据帧，数据帧经过编码、组帧等处理之后，通知串口发送任务进行发送。
*/
void pcTxQueueAddFrame(pcFrame *frame);


/*
* 向接收队列中插入一串数据
* 返回值：0 插入失败，1 插入成功
*/
u16 pcRxQueueIn(u8* buffer, u8 buflen);


/*
 * 提取帧：从接收队列中提取出一个帧，要求校验也正确
 * 返回值：0 正常取出，1 失败。
*/
u8 pcRxQueueOutFrame(pcFrame *frame);

#endif

