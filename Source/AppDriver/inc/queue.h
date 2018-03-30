/*
2014.04.12
yshy119
*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include "stm32f10x_type.h"

#define QUEUE_MAX 		256

/* ������ж��� */
typedef struct _FIFO_Queue
{
    u32 inpos;    /*���*/
	u32 outpos;    /*���*/
    u8 buffer[QUEUE_MAX];  /*��������*/
}FIFO_Queue; //lt:ʹ��ѭ������


/* ��ʼ�����У�������в��� */
void QueueInit(FIFO_Queue* fq);

/* ��ȡ���г��� */
u16 QueueGetLen(FIFO_Queue* fq);

/*
* ����в���һ���ַ�
* ����ֵ��0 ����ʧ�ܣ�1 ����ɹ�
*/
u16 QueueInByte(FIFO_Queue* fq, u8 byte);


/*
* �Ӷ�����ȡ��һ���ַ�
* ����ֵ��0 ȡ��ʧ�ܣ�1 ȡ���ɹ�
*/
u16 QueueOutByte(FIFO_Queue* fq, u8* byte);


/*
* ����в���һ���ַ���
* ����ֵ��0 ����ʧ�ܣ�1 ����ɹ�
*/
u16 QueueIn(FIFO_Queue* fq, u8* buffer, u8 len);

/*
* �Ӷ�����ȡ��һ���ַ���
* ����ֵ��0 ȡ��ʧ�ܣ�1 ȡ���ɹ�
*/
u16 QueueOut(FIFO_Queue* fq, u8* buffer, u8 len);

#endif
