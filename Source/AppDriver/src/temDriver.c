/*
temperature_drive.c

恒温系统底层驱动实现
主要用于定义加热、制冷引脚，实现加热与制冷控制。

*/

#include "temDriver.h"


TEM_IO_CTRL g_temx_io[TEM_NUM];


/*
加热制冷控制
加热：C1 = 0 ， C2 = 1
制冷：C1 = 1 ， C2 = 0
关闭：C1 = 0 ， C2 = 0  (两个都等于1时也是停止)

SD = 0 时，停止工作
SD = 1 时，启动工作
*/



/* 
初始化
将所有温度控制引脚设置为OUT_PP模式
*/
void temPinInit(void)
{							   
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
   
	/* 温控输出用到了 PD5-PD7 */
	GPIO_InitStructure.GPIO_Pin = TEM_PIN_C1 | TEM_PIN_C2 | TEM_PIN_SD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	g_temx_io[TEM1].C1 = TEM_PIN_C1;//lt:PD7
	g_temx_io[TEM1].C2 = TEM_PIN_C2;//lt:PD6
	g_temx_io[TEM1].SD = TEM_PIN_SD;//lt:PD5


	/* 关闭所有温控 */
	for(i = TEM1; i < TEM_NUM; i++)
	{
		temStop(i);
	}
}



/* 
加热
根据输入的参数使对应的温控加热
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temHeat(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 1 关闭 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 0 */
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 1 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].SD);		/* SD = 0 开启 */
	return 0;
}

/* 
制冷
根据输入的参数使对应的温控制冷
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temCool(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 0 关闭 */
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 1 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 0 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].SD);		/* SD = 1 开启 */ //lt:高电平时HO和LO符合IN时序
	return 0;
}


/* 
停止
根据输入的参数使对应的温控停止
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temStop(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 0 关闭 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 0 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 0 */
	return 0;
}


