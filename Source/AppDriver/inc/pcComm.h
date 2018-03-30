
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
��PC���Ĵ��ڷ��Ͷ�����д��һ��ԭʼ����֡������֡�������롢��֡�ȴ���֮��֪ͨ���ڷ���������з��͡�
*/
void pcTxQueueAddFrame(pcFrame *frame);


/*
* ����ն����в���һ������
* ����ֵ��0 ����ʧ�ܣ�1 ����ɹ�
*/
u16 pcRxQueueIn(u8* buffer, u8 buflen);


/*
 * ��ȡ֡���ӽ��ն�������ȡ��һ��֡��Ҫ��У��Ҳ��ȷ
 * ����ֵ��0 ����ȡ����1 ʧ�ܡ�
*/
u8 pcRxQueueOutFrame(pcFrame *frame);

#endif

