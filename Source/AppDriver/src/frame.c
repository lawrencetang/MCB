/*
2012.01.14
*/


#include "frame.h"
#include "protocal.h"
#include "string.h"


/* 帧初始化，清空所有数据，帧头设置为FF FF */
void frameInit(pcFrame *frame)
{
	memset((u8*) frame, 0, FRAME_LEN);
	frame->header[0] = 0xFF;
	frame->header[1] = 0xFF;
	frame->flag = UP_FRAME;//lt:0xB0
	frame->dev = 0x01;
}



/*
* 求校验：对缓冲区求异或校验。
* 返回值：校验结果。
* buffer：校验的缓冲区地址指针
* bufLen：缓冲区长度
*/
u8 getBufferVerify(u8 *buffer, u32 bufLen)
{
    u8 verify = 0;
    while(bufLen--) verify ^= buffer[bufLen];
    return verify;
}



/*
* 帧封装：加帧头，求校验。
* frame：要打包的数据帧
*/
void framePackage(pcFrame *frame)
{
    frame->header[0] = 0xFF;
	frame->header[1] = 0xFF;
	frame->flag = UP_FRAME;
	frame->verify = getBufferVerify((u8*) frame, FRAME_LEN - 1);//lt:异或了11个字节,校验了；两个0XFF，故相当于只校验了从帧标识开始的9字节
}



/*
* 检查帧合法性：判断帧头是否为FF FF，判断校验是否正确。
* 返回值：0 正确， 1 帧头错误， 2 校验错误。
*/
u32 frameCheck(pcFrame *frame)
{
	if((frame->header[0] != 0xFF) || (frame->header[1] != 0xFF))
	{
		return 1;
	}
	if(getBufferVerify((u8*) frame, FRAME_LEN) != 0)
	{
		return 2;//lt:校验12个字节，前面两个0XFF抵消，将帧标识开始的9字节异或后和校验字节异或，两个值相同结果为0，说明传输正确
	}
	return 0;
} 



