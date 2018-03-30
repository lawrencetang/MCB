/*
2014.04.12
yshy119
*/

#ifndef _FRAME_H
#define _FRAME_H

#include "stm32f10x_type.h"

#define FRAME_LEN 		12		/* 原始帧固定长度 */

/* 帧定义 */
typedef struct _pcFrame
{
    u8 header[2];   	/* 帧头 FF FF */
	u8 flag;   			/* 帧标识，下行C0，上行B0 */
	u8 cmd;   			/* 命令号 */
	u8 dev;	   			/* 设备编号 */
    u8 sts;   			/* 操作结果状态 */
	u8 para;   		/* 决定数据域是否有意义的标志：00没有数据，01带数据 */
	u8 data[4];   		/* 数据域 */
    u8 verify;    		/* 帧校验，前面所有数据有字节异域校验 */
}pcFrame;



/* 帧初始化，清空所有数据，帧头设置为FF FF */
void frameInit(pcFrame *frame);


/*
* 求校验：对缓冲区求异或校验。
* 返回值：校验结果。
* buffer：校验的缓冲区地址指针
* bufLen：缓冲区长度
*/
u8 getBufferVerify(u8 *buffer, u32 bufLen);


/*
* 帧封装：加帧头，求校验。
* frame：要打包的数据帧
*/
void framePackage(pcFrame *frame);


/*
* 检查帧合法性：判断帧头是否为FF FF，判断校验是否正确。
* 返回值：0 正确， 1 没有帧头， 2 校验错误。
*/
u32 frameCheck(pcFrame *frame);	


/*
 * 提取帧：从缓冲区中提取出一个帧，只进行帧头判断，不校验
 * 返回值：0 正常取出，1 失败。
 * frame：提取的结果，即完整的一帧
 * buffer：缓冲区
 * bufLen：buffer字符串长度
*/
u8 frameGetFrame(pcFrame *frame, u8 *buffer, u32 bufLen);

/*
* 提取有效帧：从缓冲区中提取出一个合法帧帧，帧头、校验都合法 
 * 返回值：0 正常取出，1 失败。
 * frame：提取的结果，即完整的一帧
 * buffer：缓冲区
 * bufLen：buffer字符串长度
*/
u8 frameGetValidFrame(pcFrame *frame, u8 *buffer, u32 bufLen);


#endif






