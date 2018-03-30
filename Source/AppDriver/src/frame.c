/*
2012.01.14
*/


#include "frame.h"
#include "protocal.h"
#include "string.h"


/* ֡��ʼ��������������ݣ�֡ͷ����ΪFF FF */
void frameInit(pcFrame *frame)
{
	memset((u8*) frame, 0, FRAME_LEN);
	frame->header[0] = 0xFF;
	frame->header[1] = 0xFF;
	frame->flag = UP_FRAME;//lt:0xB0
	frame->dev = 0x01;
}



/*
* ��У�飺�Ի����������У�顣
* ����ֵ��У������
* buffer��У��Ļ�������ַָ��
* bufLen������������
*/
u8 getBufferVerify(u8 *buffer, u32 bufLen)
{
    u8 verify = 0;
    while(bufLen--) verify ^= buffer[bufLen];
    return verify;
}



/*
* ֡��װ����֡ͷ����У�顣
* frame��Ҫ���������֡
*/
void framePackage(pcFrame *frame)
{
    frame->header[0] = 0xFF;
	frame->header[1] = 0xFF;
	frame->flag = UP_FRAME;
	frame->verify = getBufferVerify((u8*) frame, FRAME_LEN - 1);//lt:�����11���ֽ�,У���ˣ�����0XFF�����൱��ֻУ���˴�֡��ʶ��ʼ��9�ֽ�
}



/*
* ���֡�Ϸ��ԣ��ж�֡ͷ�Ƿ�ΪFF FF���ж�У���Ƿ���ȷ��
* ����ֵ��0 ��ȷ�� 1 ֡ͷ���� 2 У�����
*/
u32 frameCheck(pcFrame *frame)
{
	if((frame->header[0] != 0xFF) || (frame->header[1] != 0xFF))
	{
		return 1;
	}
	if(getBufferVerify((u8*) frame, FRAME_LEN) != 0)
	{
		return 2;//lt:У��12���ֽڣ�ǰ������0XFF��������֡��ʶ��ʼ��9�ֽ������У���ֽ��������ֵ��ͬ���Ϊ0��˵��������ȷ
	}
	return 0;
} 



