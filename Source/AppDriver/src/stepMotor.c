/*
stepMotor.c
*/

#include "protocal.h"
#include "task.h"
#include "stepMotor.h"
#include "math.h"
#include "TaskInit.h"
#include "pcComm.h"
#include "absEncoder.h"
#include "eeprom.h"

#include <includes.h>

/*
转动控制参数，一次转动操作过程中，用于控制加速、匀速、减速操作的
*/
STEP_MOTOR g_sm[SM_NUM];


/*
步进电机初始化，包括引脚和参数
*/
void smInit(void)
{	
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
   
	/* 步进电机1用的是PD13~PD15, 步进电机2用的是PD10~PD12 */
	GPIO_InitStructure.GPIO_Pin = SM1_PIN_EN | SM1_PIN_STP | SM1_PIN_DIR | SM2_PIN_EN | SM2_PIN_STP | SM2_PIN_DIR;//lt:PE8-10,PE13-15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 	 	

	/*步进电机IO输出配置*/
	g_sm[0].pin.en 	= SM1_PIN_EN;//lt:控制器使能信号
	g_sm[0].pin.stp = SM1_PIN_STP;//lt:控制器步进脉冲
	g_sm[0].pin.dir = SM1_PIN_DIR;//lt:控制器方向信号
	g_sm[0].para.event = OSSemCreate(0);;//lt:建立并初始化一个信号量,必须有释放该事件的信号才能执行

	g_sm[1].pin.en 	= SM2_PIN_EN;
	g_sm[1].pin.stp = SM2_PIN_STP;
	g_sm[1].pin.dir = SM2_PIN_DIR;
	g_sm[1].para.event = OSSemCreate(0);;

	absEncInit();


	/*初始化控制参数*/
	for (i = 0; i < SM_NUM; i++)
	{
		smClearRotate(i); 		

		g_sm[i].flag = SM_STS_IDLE;//lt:电机状态空闲
		g_sm[i].goalAngle = 0;//lt:要转动到的目标角度
		g_sm[i].para.accTicks = SM_ACC_TICKS; 	/* 加速度 *///lt:0.005
		g_sm[i].para.maxTicks = SM_MAX_WAIT_TICKS;//lt:10
		g_sm[i].para.minTicks = SM_MIN_WAIT_TICKS;//lt:2
		
		smUpdateAngle(i);//lt:采用编码器得到真实的角度
	} 
}

/*
填充步进电机角度并上传数据帧 
*/
void smUploadAngle(pcFrame *frame)
{
	frame->sts = STS_SUCCESS;//lt:操作状态0x00
	frame->para = 1;//lt:携带参数		
	frame->data[0] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>24;//lt:实际旋转角度高位
	frame->data[1] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>16;
	frame->data[2] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>8;
	frame->data[3] = ((u32)(g_sm[frame->dev-1].realAngle*10));//lt:实际旋转角度*10上传
	pcTxQueueAddFrame(frame);	
}

void smUploadLogicZero(pcFrame *frame)
{
	float logicZero;
	logicZero = absEncGetLogicZero(frame->dev-1);
	frame->sts = STS_SUCCESS;
	frame->para = 1;		
	frame->data[0] = ((u32)logicZero*10)>>24;//lt:逻辑0°高位
	frame->data[1] = ((u32)logicZero*10)>>16;
	frame->data[2] = ((u32)logicZero*10)>>8;
	frame->data[3] = ((u32)logicZero*10);//lt:逻辑0°*10上传
	pcTxQueueAddFrame(frame);	
}


/*
*/
u8 smSetCMD(pcFrame frame)
{
	u8 dev_num = frame.dev - 1;
	if(dev_num >= SM_NUM) /*电机编号不合法*/
		return STS_DEV_ERROR; 
	if(g_sm[dev_num].flag == SM_STS_BUZY)  //lt:01,电机状态繁忙
		return STS_BUZY;
	if(g_sm[dev_num].flag)  //lt:故障
		return STS_FAULT; 

	switch(frame.cmd)
	{																					 
		case SM_CHECK:			//电机故障查寻  //lt:命令号0x80
			smUploadAngle((pcFrame *)&frame);//lt:读旋转角度
			break;

		case SM_START:		   	//启动电机，电机2不允许转动到340度以上的区域		//lt:命令号0x81					
			g_sm[dev_num].goalAngle = (float)((frame.data[0]<<24 | frame.data[1]<<16 | frame.data[2]<<8 | frame.data[3])/10.0);//lt:根据下发指令确定目标角度,并还原角度的真实值
			if((g_sm[frame.dev].goalAngle >= SM_MAX_ANGLE) || ((frame.dev == SM2) && (g_sm[frame.dev].goalAngle > 340)))//lt:未将设备号-1是否能够对应数组?
			{
				g_sm[dev_num].goalAngle = 0;
				return STS_DATA_ERROR;//lt:返回0x05
			}

			g_sm[dev_num].flag = SM_STS_BUZY;//lt:设置为忙标志
			OSSemPost(g_sm[dev_num].para.event);//lt:初始信号数量为0,可执行一个HPT(被睡眠的步进电机1或步进电机2任务)
			break;

		case SM_STOP:		   	//强制停止电机  //lt:命令号0x82
			g_sm[dev_num].ro.sts = 0;//lt:单次停止转动
			g_sm[dev_num].flag = SM_STS_IDLE;
			smUploadAngle((pcFrame *)&frame);//lt:上传实际旋转角度
			break;

		case SM_READAPOS:		//读当前角度位置 //lt:命令号0x83
			smUploadAngle((pcFrame *)&frame);//lt:上传上一次的实际旋转角度
			smUpdateAngle(dev_num);//lt:更新当前设备的实际旋转角度
			break;
		case SM_READZERO:  //lt:读逻辑0点，命令号0x84
			smUploadLogicZero((pcFrame *)&frame);
			break;
		case SM_SETAERO:  //lt:设置逻辑0点，命令号0x85
			absEncSetLogicZero(dev_num);//lt:将绝对角度设置为逻辑0点保存到相应设备的EEprom的相应地址中
			smUploadLogicZero((pcFrame *)&frame);//lt:上传逻辑0点
			break;

		default:
			return STS_CMD_ERROR; //lt:返回0x03，指令错误
	}
	
	return STS_SUCCESS;	//lt:返回0x00，指令成功
}




/*
清除步进电机的所有单次转动相关的参数
*/
void smClearRotate(u8 num)
{
	g_sm[num].ro.sts = 0;//lt:单次转动状态指示，0 转动停止， 1 继续转动
	g_sm[num].ro.totalPulse = 0;
	g_sm[num].ro.remainPulse = 0;
	g_sm[num].ro.ticks = 0;//lt:实时脉冲计数
}


/*
启动电机转动
num：电机编号
pulse：转动脉冲数量
acc：脉冲加速度
*/
void smStartSM(u8 num, u32 pulse, float acc)
{
	if(g_sm[num].flag == SM_STS_IDLE)
	{
		return ;
	}
	g_sm[num].ro.acc = acc;//lt:加速度
	g_sm[num].ro.remainPulse = pulse;//lt:剩余脉冲数	
	g_sm[num].ro.ticks = g_sm[num].ro.spdTicks;//lt:对脉冲计数
	g_sm[num].ro.sts = 1;//lt:继续转动	
}


/*
加速转动
num：电机编号
pulse：转动脉冲数量
*/
u8 smSpeedUp(u8 num, u32 pulse)
{
	u8 err;
	g_sm[num].ro.spdTicks = g_sm[num].para.maxTicks;
	smStartSM(num, pulse, 0-g_sm[num].para.accTicks);//lt:第三个参数为负数
	OSSemPend(g_sm[num].para.event, 5000, &err);//lt:请求信号,延时时限为0.5s
	return err;
}

/*
减速转动
num：电机编号
pulse：转动脉冲数量
*/
u8 smSlowDown(u8 num, u32 pulse)
{
	u8 err;
	smStartSM(num, pulse, g_sm[num].para.accTicks);//lt:第三个参数为正数
	OSSemPend(g_sm[num].para.event, 5000, &err);//ltnotice：是否要提前至smstartsm函数调用前来卡住acc变量的改变?
	return err; //lt:返回0无错
}


/*
匀速转动
num：电机编号
pulse：转动脉冲数量
*/
u8 smUniform(u8 num, u32 pulse)
{
	u8 err;
	smStartSM(num, pulse, 0);
	OSSemPend(g_sm[num].para.event, 5000, &err);
	return err;
}


/*
电机运行控制
num：电机编号
degree：要转动的角度，其中转动方向在dir里面。
*/
u8 smRunSM(u8 num, float degree)
{
	u32 accPulse;
	
	if(degree < SM_EQUAL_ANGLE_ERROR)
	{
		return STS_SUCCESS;
	}//lt:闭环判断
	
	SM_CLREN(num);//lt:失能
	SM_SETDIR(num);//lt:通过结构体中的dir控制电机转动方向
	g_sm[num].ro.totalPulse = SM_DegreeToPulse(degree)*2;//lt:stp口翻转次数
	accPulse = 0;
	if(fabs(g_sm[num].para.accTicks) > 0.0001)  //lt:浮点类型求绝对值,0.005
	{
		accPulse = (g_sm[num].para.maxTicks - g_sm[num].para.minTicks)/g_sm[num].para.accTicks;  //lt:1600,从最大半周期到最小半周期的翻转次数
	}

	if(g_sm[num].ro.totalPulse <= (accPulse * 2))  //lt:小于180°
	{
		accPulse = g_sm[num].ro.totalPulse / 2;
		if(smSpeedUp(num, accPulse))
		{
			return SM_STS_FAULT;
		}//lt:加速
		if(smSlowDown(num, accPulse))
		{
			return SM_STS_FAULT;
		}//lt：减速
	}
	else
	{
		if(smSpeedUp(num, accPulse))
		{
			return SM_STS_FAULT;
		}
		if(smUniform(num, g_sm[num].ro.totalPulse - 2 * accPulse))
		{
			return SM_STS_FAULT;
		}
		if(smSlowDown(num, accPulse))
		{
			return SM_STS_FAULT;
		}
	}//lt:加速减速匀速
	return STS_SUCCESS;
}

/*
停止电机转动
*/
void smStopSM(u8 num)
{
	smClearRotate(num);
}


/*
步进电机控制的tick
本函数通过定时器调用，要求定时器的时间间隔是20us，即每20us会进入本函数一次。
每次控制都需要经过加速和减速过程
*/
void smRotateTick(void)
{
	u8 i;
	for(i = 0; i < SM_NUM; i++)
	{
		if(g_sm[i].ro.sts)	/* 本次转动是否结束，如果转动未结束则进入本条件 */
		{
			if(!g_sm[i].ro.ticks--)   	/* 定时是否到 */ //lt:到达等于0
			{
				if(g_sm[i].ro.remainPulse--)	/* 脉冲是否达到 */ //lt:没到大于0
				{
					g_sm[i].ro.spdTicks = g_sm[i].ro.spdTicks + g_sm[i].ro.acc;
					if(g_sm[i].ro.spdTicks < g_sm[i].para.minTicks)
					{
						g_sm[i].ro.spdTicks = g_sm[i].para.minTicks;
					}//lt:控死最小限脉冲频率,匀速时最快为2
					else if(g_sm[i].ro.spdTicks > g_sm[i].para.maxTicks)
					{
						g_sm[i].ro.spdTicks = g_sm[i].para.maxTicks;
					}//lt:控死最大限脉冲频率,下次加速最慢为10
					SM_REVSTP(i);//lt:stp口翻转
					smUpdateAngle(i);//lt:更新角度
				}
				else
				{
					g_sm[i].ro.sts = 0;//lt:到达脉冲计数停止转动
					OSSemPost(g_sm[i].para.event);
				}
				g_sm[i].ro.ticks = g_sm[i].ro.spdTicks;//lt:加速使数20us的个数越来越少，减速使数20us的个数越来越多
			}			
		}
		else
		{
			smUpdateAngle(i);
		}		
	}
}



/*
* 获取要转动的角度大小，并自动取得转动方向
* 通过目标角度与当前角度计算还需要转动的角度
*/
float smGetAngle(u8 num)
{
    float angle;
	if(num == SM1)
	{
		angle = g_sm[num].goalAngle - g_sm[num].realAngle;	
		if (angle < 0)
		{
			angle = angle + 360;
		}	
		/* 从当前角度转到目标角度，逆时针，角度小于等于180则正向转动，否则反向转动 */
	    g_sm[num].dir = 0;		/* 正向，逆时针 */
		if(angle > 180)
		{
			g_sm[num].dir = 1;	/* 负向，顺时针 */
			angle = 360 - angle;
		}
	}
	else if(num == SM2)
	{
		if(g_sm[num].realAngle > 350) 	/* 350度到360度之间的当作0度处理 */
		{
			g_sm[num].dir = 0;
			angle = g_sm[num].goalAngle + 360 - g_sm[num].realAngle;
		}
		else
		{
			if(g_sm[num].goalAngle > g_sm[num].realAngle)
			{
				g_sm[num].dir = 0;
				angle = g_sm[num].goalAngle - g_sm[num].realAngle;
			}
			else
			{
				g_sm[num].dir = 1;
				angle = g_sm[num].realAngle - g_sm[num].goalAngle;
			}
		}
	}
	else
	{
		angle = 0;
	}
	return angle;
}



/*
转动控制
电机的转动控制被拆为两三次转动，加速、匀速、减速。如果转动角度较小，则只有加速和减速
每次
*/
u8 smRotate(u8 num)
{
	u8 err;
	pcFrame frame;

	err = smRunSM(num, smGetAngle(num));//lt:成功是0.不成功是2
//	err += smRunSM(num, smGetAngle(num));
	g_sm[num].flag = SM_STS_IDLE;

	frame.cmd = SM_START;
	frame.dev = num + 1;
	frame.sts = (err + smJudgeEqualAngle(g_sm[num].realAngle, g_sm[num].goalAngle));
	frame.para = 1;

	frame.data[0] = ((u32)(g_sm[num].realAngle*10))>>24;
	frame.data[1] = ((u32)(g_sm[num].realAngle*10))>>16;
	frame.data[2] = ((u32)(g_sm[num].realAngle*10))>>8;
	frame.data[3] = ((u32)(g_sm[num].realAngle*10));

	pcTxQueueAddFrame((pcFrame *)&frame);
	return err;
}


/*
判断两个角度是否相等
如果两个角度差小于SM_EQUAL_ANGLE_ERROR，则相等，否则判断为不等
返回值：1 不等，0 相等
*/
u8 smJudgeEqualAngle(float a1, float a2)
{
	float a;
	a = a1 - a2;
	if(a1 < a2)
	{
		a = a2 - a1;
	}
	
	if(a > 180)
	{
		a = 360 - a;
	}
	return (a > SM_EQUAL_ANGLE_ERROR);//lt:超过1°返回1
}

/*
步进电机自检
让电机转动10度，再转回原来的位置，判断转动是否正常，如果正常则返回成功，否则返回失败。
*/
u8 smCheck(u8 num)
{
	float destAngle, nowAngle;
	if(g_sm[num].flag == SM_STS_BUZY)
	{
		return STS_BUZY;						  
	}
	nowAngle = g_sm[num].realAngle;
	g_sm[num].goalAngle = g_sm[num].realAngle + 10;
	if(g_sm[num].goalAngle > 360)
	{
		g_sm[num].goalAngle = g_sm[num].goalAngle - 360;
	}
	destAngle = g_sm[num].goalAngle;
	smRotate(num);
	if(smJudgeEqualAngle(destAngle, g_sm[num].realAngle))
	{
		return STS_FAULT;
	}
	g_sm[num].goalAngle = nowAngle;
	smRotate(num);
	if(smJudgeEqualAngle(nowAngle, g_sm[num].realAngle))
	{
		return STS_FAULT;
	}
	return STS_SUCCESS; 
}

/*
* 更新步进电机的角度
*/
void smUpdateAngle(u8 num)
{
	absEncUpdateAngle(num);//lt:通过编码器计算出实际旋转角度
	g_sm[num].realAngle = absEncGetAngle(num);//lt:将实际旋转角度赋予结构
}


