/*
2014.04.12
yshy119
*/

#ifndef _FRAME_H
#define _FRAME_H

#include "stm32f10x_type.h"

#define FRAME_LEN 		12		/* ԭʼ֡�̶����� */

/* ֡���� */
typedef struct _pcFrame
{
    u8 header[2];   	/* ֡ͷ FF FF */
	u8 flag;   			/* ֡��ʶ������C0������B0 */
	u8 cmd;   			/* ����� */
	u8 dev;	   			/* �豸��� */
    u8 sts;   			/* �������״̬ */
	u8 para;   		/* �����������Ƿ�������ı�־��00û�����ݣ�01������ */
	u8 data[4];   		/* ������ */
    u8 verify;    		/* ֡У�飬ǰ�������������ֽ�����У�� */
}pcFrame;



/* ֡��ʼ��������������ݣ�֡ͷ����ΪFF FF */
void frameInit(pcFrame *frame);


/*
* ��У�飺�Ի����������У�顣
* ����ֵ��У������
* buffer��У��Ļ�������ַָ��
* bufLen������������
*/
u8 getBufferVerify(u8 *buffer, u32 bufLen);


/*
* ֡��װ����֡ͷ����У�顣
* frame��Ҫ���������֡
*/
void framePackage(pcFrame *frame);


/*
* ���֡�Ϸ��ԣ��ж�֡ͷ�Ƿ�ΪFF FF���ж�У���Ƿ���ȷ��
* ����ֵ��0 ��ȷ�� 1 û��֡ͷ�� 2 У�����
*/
u32 frameCheck(pcFrame *frame);	


/*
 * ��ȡ֡���ӻ���������ȡ��һ��֡��ֻ����֡ͷ�жϣ���У��
 * ����ֵ��0 ����ȡ����1 ʧ�ܡ�
 * frame����ȡ�Ľ������������һ֡
 * buffer��������
 * bufLen��buffer�ַ�������
*/
u8 frameGetFrame(pcFrame *frame, u8 *buffer, u32 bufLen);

/*
* ��ȡ��Ч֡���ӻ���������ȡ��һ���Ϸ�֡֡��֡ͷ��У�鶼�Ϸ� 
 * ����ֵ��0 ����ȡ����1 ʧ�ܡ�
 * frame����ȡ�Ľ������������һ֡
 * buffer��������
 * bufLen��buffer�ַ�������
*/
u8 frameGetValidFrame(pcFrame *frame, u8 *buffer, u32 bufLen);


#endif






