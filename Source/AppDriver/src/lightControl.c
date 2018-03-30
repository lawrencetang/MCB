/*
lightControl.c
氙灯控制与积分时间控制。
氙灯和CMOS的积分时间由定时器控制
定时器固定周期为100us，点灯周期固定为15ms。
*/

#include "lightControl.h"

lightCtrl g_lightCtrl;
cmosCtrl g_cmosCtrl;;

void lightInit(void)
{
	g_lightCtrl.flag = 0;
	g_lightCtrl.counter = 0;
	g_lightCtrl.period = FrequentToIrqCnt(LIGHT_MAX_FREQUENT); /* 设置点灯周期，即点灯频率，可以从EEPROM中读取，也可以固定 */

	if(g_lightCtrl.period < FrequentToIrqCnt(LIGHT_MAX_FREQUENT))
	{
		g_lightCtrl.period = FrequentToIrqCnt(LIGHT_MAX_FREQUENT);
	}

	g_cmosCtrl.flag = 0;
	g_cmosCtrl.counter = 0;
	g_cmosCtrl.totalCnt = TimeToIrqCnt(ST_DEFAULT_TIME); 	/* 积分时间 */
}

/* 定时器中断调用本函数 */
void lightCtrlTick(void)
{	
	g_lightCtrl.counter++;

	if(g_lightCtrl.counter >= (g_lightCtrl.period - 1))
	{
		g_lightCtrl.counter = 0;
		if(g_lightCtrl.flag)
		{/* 打开灯控制引脚，开灯 */
		}	
	}
	else if(g_lightCtrl.counter >= (g_lightCtrl.period/2))
	{
		/* 关闭灯控制引脚，关灯 */
	}

	if(g_cmosCtrl.flag)
	{/* CMOS正在进行数据采集 */
		if(g_cmosCtrl.counter < g_cmosCtrl.totalCnt)
		{
			g_cmosCtrl.counter++;
		}
		else
		{
			/* CMOS积分时间到，ST = 0 */
		}
	}
	else
	{/* CMOS空闲，判断点灯时间，点灯的开始后的特定时间启动一次采集数据 */
		if(g_lightCtrl.counter == ST_DELAY)
		{
			g_cmosCtrl.flag = 1; 	/* 在2048点数据都取完成时，自动将该标识置0 */
			g_cmosCtrl.counter = 0;
			/* CMOS开始积分，ST = 1 */	
		}
	}
}






