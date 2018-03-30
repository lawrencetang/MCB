/*
temControl.c

温度控制应用层

*/
#include "temControl.h"
#include "protocal.h"
#include "math.h"
#include "adcDriver.h"
#include "common.h"


TEM_CTRL g_tem[TEM_NUM];  //lt:TEM_NUM = 1


/* 恒温控制应用层初始化 */
u8 temAppInit(void)
{
	u8 i;
	/* 清除所有参数 */
	for(i = TEM1; i < TEM_NUM; i++)   //lt:TEM1 = 0
	{
		temAppClear(i);/* 清除参数 */
		g_tem[i].ad_channel = i;  /* AD通道号 */
		g_tem[i].Event = OSSemCreate(0); /* 初始化信号量 */
	}
	adcInit();
	temPinInit();
	return 0;
}


/*
清除所有温度控制参数
*/
u8 temAppClear(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	g_tem[p_id].start = 0;
	g_tem[p_id].status = 0;

	g_tem[p_id].dt = 0.0;
	g_tem[p_id].e0 = 0.0;
	g_tem[p_id].e1 = 0.0;
	g_tem[p_id].es = 0.0; 	

	return 0;
}

/*
* PT1000电阻值与温度之间的关系计算
* 温度TEM与PT1000电阻值RPT的关系为 RPT = 1000 * (1 + A * TEM + B * TEM * TEM)，其中 A = 0.00386231, B = -0.000000653149
* 通过计算，得到温度到电阻值的一个公式，再用Excel或Matlab工具，拟合反推，得到从电阻到温度的公式
* TEM = -245.898 + 0.235898 * RPT + 0.00001 * RPT * RPT
*/

#define VADREF 			3.3
#define VBREAGE 		4.96
#define RUP 	 		1000.0
#define RDOWN 			1000.0

float g_temUpload = 0; 	/* 用于上传的当前温度 */

/* 
* 获取当前温度，先通过AD的值反推出PT1000的电阻值，再由电阻值计算出温度。
* 采集到的12位AD值是电压的数字量，转换为AD的输入电压为 VAD = 3.3 * data / 4095  //lt:有参考电压
* AD输入的电压VAD是电桥的差分电压经过放大得到，放大倍数为GAIN，则差分电压 VDIV = VAD / GAIN
* 差分电压VDIV是由电桥分压得到，通过三个1K电阻和一个PT1000组成电桥，VDIV = VIP - VIN
* 电桥同相端电压为 VIP = 2.5V
* 电桥向相端电压为 VIN = 5 * 1000 / (1000 + RPT)，其中RPT是PT1000的电阻值。
* RPT = 5000 / VIN - 1000
*/
float temAppGetTem(u8 p_id)
{
	u16 l_ad; 	  				//AD值	
	float vad, vdiv, vin; 	//电压
	float rpt;			  		//电阻值
	float tem; 		//温度
 
	if(p_id >= TEM_NUM) 		//多个温度控制系统时才用
	{
		return -1000.0;  //错误
	}//lt:p_id = 0

	l_ad = g_ad.tem;
	vad = VADREF * l_ad / 4095;
	vdiv = vad / GAIN; //lt:gain为1+(53.6k/10k) = 6.36
	vin = VBREAGE * RDOWN / (RUP + RDOWN) - vdiv;
	rpt = RDOWN * VBREAGE  / vin - RDOWN;
	tem = 0.00001 * rpt * rpt + 0.235898 * rpt - 245.898; 

	g_temUpload = (g_temUpload * 9.0 + tem)	/ 10.0;

	return tem;
}



/*
* 根据目标温度与当前温度计算加热制冷时间
* 总时间为：TEM_TOTAL_TIME
* 当目标温度大于当前温度时，返回正数，当目标温度小于当前温度时，返回负数
*/
s16 temGetHighTime(u8 p_id)
{
	float l_nt;
	s16 high_time; 
	l_nt = temAppGetTem(p_id);
	if(l_nt < -100.0)	
	{
		return -TEM_TOTAL_TIME;
	}

	g_tem[p_id].e1 = g_tem[p_id].e0;
	g_tem[p_id].e0 = g_tem[p_id].dt - l_nt;//lt:目标温度与当前温度的差
	g_tem[p_id].es = g_tem[p_id].es + g_tem[p_id].e0;//lt:当前积分项

	if(fabs(g_tem[p_id].e0) > TEM_PID_VALID)  //lt:误差小于0.2°进行PID。大于0.2°直接返回tem_total_time
	{
		g_tem[p_id].es = 0;
		if(g_tem[p_id].e0 > 0)
		{
			return TEM_TOTAL_TIME;//lt:误差大于0，即目标温度大于当前温度
		}
		else
		{
			return -TEM_TOTAL_TIME;
		}
		
	}

	high_time = KP * g_tem[p_id].e0 + KI * g_tem[p_id].es + KD * (g_tem[p_id].e0 - g_tem[p_id].e1);//lt:PID公式：KP=2.5，KI=0.05，KD=0.01

	if(high_time > TEM_TOTAL_TIME)
	{
		high_time = TEM_TOTAL_TIME; 
	}

	if(high_time < -TEM_TOTAL_TIME)
	{
		high_time = -TEM_TOTAL_TIME;
	}

	return high_time;
}//lt:误差在0.2°内得到的pid时间在tem_total_time正负范围内返回



/*
* 加热制冷控制
* 在每 TEM_TOTAL_TIME 时间内进行闭环控制，导通的时间由PID算法计算得出 
*/
u8 temHeatCool(u8 p_id)
{
	s16 l_high_time; 	/* 加热制冷持续的电平，由TEM_GetHighTime函数得到 */
	u16 l_hc_delay;		/* 加热或制冷持续时间，总时间为 TEM_TOTAL_TIME，导通时间为 l_high_time，剩余便是断开时间 */
	u32 l_cycle_cnt; 	/* 闭环控制的总轮数 */

	float l_tem_err;
	
	if(p_id >= TEM_NUM)
	{
		return STS_DEV_ERROR;  //错误
	} 

	g_tem[p_id].e0 = 0;
	g_tem[p_id].e1 = 0;
	g_tem[p_id].es = 0;
	l_hc_delay = 0;
	l_cycle_cnt = 0;
	l_tem_err = 1000.0;
	
	while(g_tem[p_id].start)
	{
		FANON;	 			//保证风扇打开
			
		l_high_time = temGetHighTime(p_id);	//获取一个周期内，加热的时间

		if(l_high_time < -TEM_TOTAL_TIME)
		{
			temStop(p_id);
			temAppClear(p_id);
			return STS_FAULT;	 //出现错误，直接返回
		}

		if(l_high_time < 0) 	//加热的时间小于0，即温度过高，需要制冷
		{
			l_hc_delay = 0 - l_high_time;
			temCool(p_id);  	//如果没有制冷功能，这句话需要屏蔽掉，即只提供加热功能//lt:调用制冷功能，时间l_hc_delay
		}
		else					//如果大于0则需要加热
		{
			l_hc_delay = l_high_time;
			temHeat(p_id);//lt:调用制热功能，时间l_hc_delay
		}

		OSTimeDly(l_hc_delay);	//加热high_time的时间//lt:或制冷

		temStop(p_id);	 		//停止 

		OSTimeDly(TEM_TOTAL_TIME - l_hc_delay);//等待一个周期完成

	   	//加热 TEMP_CYCLE_CTRL 个周期后，需要判断温度变化情况，如果温度不变，则认为异常，立即停止加热控制
		if(0 == (l_cycle_cnt % TEM_CYCLE_CTRL))   //lt:tem_cyle_ctrl = 1200
		{//离目标温度正负 TEMP_VALID 以外，当前温度比上次温度离目标温度要求更近				
			if((fabs(g_tem[p_id].e0) > TEM_DIS_DIF) && (fabs(l_tem_err) - fabs(g_tem[p_id].e0) < TEM_VALID_ERR))//lt:误差大于5°且本次误差比上次误差只小1°(基本不变)
			{
				g_tem[p_id].start = 0; //停止恒温控制
				return STS_FAULT;
			}
			l_tem_err = g_tem[p_id].e0;	//每一段时间记录一下当前温度与目标温度的差 
		}

		l_cycle_cnt++; //温度控制轮数
	}
	temStop(p_id);
	temAppClear(p_id);		
	return STS_SUCCESS;	
}


/*
* 温控指令处理程序
* 处理上位机下发的温度控制指令
*/
u8 temCMDDeal(pcFrame frame)
{
	u8 l_id;
	s32 l_tem;

	l_id = frame.dev - 1;  /* 温控设备编号 */ 
	if(l_id >= TEM_NUM)
	{
		return STS_DEV_ERROR; /* 编号不对 */
	}

	switch(frame.cmd)
	{
	case TEM_CHECK:
		break;

	case TEM_START:			/* 启动恒温 */ 
		if(1 == g_tem[l_id].start)
		{
			return STS_BUZY; /* 已在工作 */
		}
		g_tem[l_id].start = 1;
		g_tem[l_id].status = 0;
		g_tem[l_id].dt = (frame.data[2]*256 + frame.data[3])/10.0;/* 温度精度0.1℃ */
		if((g_tem[l_id].dt > TEM_MAX_TEMP) || (g_tem[l_id].dt < TEM_MIN_TEMP))
		{
			temAppClear(l_id);
			return STS_DATA_ERROR;
		}		
		OSSemPost(g_tem[l_id].Event);
		break;

	case TEM_STOP:				/* 停止恒温 */
//		if(0 == g_tem[l_id].start)
//		{
//			return STS_FAILED; /* 不在工作 */
//		}
		temAppClear(l_id);
		break;
	case TEM_READ:				/* 读取温度 */
		break;
	default:
		return STS_CMD_ERROR;
	}

	l_tem = 10 * g_temUpload;  /* 温度精度为0.1℃，因此需要乘以10 */

	frame.data[0] = 0;
	frame.data[1] = 0;
	frame.data[2] = (l_tem>>8) & 0xff;
	frame.data[3] = l_tem & 0xff;
	frame.para = 1;
	
	pcTxQueueAddFrame((pcFrame *)&frame); 
		
	return STS_SUCCESS;
}

/*
* 恒温控制任务函数
*/
void temTaskDeal(u8 p_id)
{
	s32 l_tem;
	pcFrame frame;
	frameInit((pcFrame *)&frame);
	FANON;
	frame.sts = temHeatCool(p_id);
	FANOFF;
	if(frame.sts)
	{		
		frame.cmd = TEM_STOP;
		frame.dev = p_id + 1;
		frame.para = 0x01;
		l_tem = 10 * g_temUpload;  /* 温度精度为0.1℃，因此需要乘以10 */ 
		frame.data[0] = 0;
		frame.data[1] = 0;
		frame.data[2] = (l_tem>>8) & 0xff;
		frame.data[3] = l_tem & 0xff;
		pcTxQueueAddFrame((pcFrame *)&frame);
	}
	temAppClear(p_id);
}



