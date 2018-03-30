/*

*/

#include "pcComm.h"
#include "dcMotor.h"
#include "protocal.h"
#include "timer5.h"
#include "stm32f10xDriver.h"


DCMotor g_dcm[DCM_NUM];

const u16 g_dcm_rate[DCM_RATE_NUM] = {DCM_RATE1, DCM_RATE2, DCM_RATE3};

/*
* 直流电机初始化
*/
void dcmInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Pin = DCM1_PIN_OUT | DCM2_PIN_OUT;//lt:PA2,PA3

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//lt:复用推挽？
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	dcmStopDCM(DCM1);
	dcmStopDCM(DCM2);

	g_dcm[DCM1].sts = 0;
	g_dcm[DCM1].rate = DCM_RATE1;//lt:1%,即高电平占0.1ms，周期10ms

	g_dcm[DCM2].sts = 0;
	g_dcm[DCM2].rate = DCM_RATE1;

	tim5SetPeriod(DCM_CNT_NUM);//lt:dcm_cnt_mum=10000,溢出时间为10ms,即PWM整个波周期为10ms，可通过函数参数设定设定PWM的占空比
	tchSWInit();
}

/*
* 直流电机命令解析
* 解析上位机发下来的指令，操作直流电机。
*/
u8 dcmSetCMD(pcFrame frame)
{
	u8 dev_num = frame.dev - 1;
	if(dev_num >= DCM_NUM) /*电机编号不合法*/
		return STS_DEV_ERROR; 

	switch(frame.cmd)
	{																					 
		case DCM_CHECK:			//电机故障查寻//lt:0x90
			break;

		case DCM_START:		   	//启动电机//lt:0x91
			if((frame.data[3] < 1) || (frame.data[3] > DCM_RATE_NUM))
			{
				return STS_DATA_ERROR;
			}
			g_dcm[dev_num].rate = g_dcm_rate[frame.data[3] - 1];
			dcmStartDCM(dev_num);							
			break;

		case DCM_STOP:		   	//停止电机//lt:0x92
			dcmStopDCM(dev_num);
			break;

		default:
			return STS_CMD_ERROR;
	}	
	return STS_SUCCESS;	
}

/*
* 启动直流电机
* 启动PWM波输出，把定时器对应通道的PWM比较值设置为相应的占空比参数
*/
void dcmStartDCM(u8 num)
{
	
	switch(num)
	{
	case DCM1:
		tim5ConfigPWM(3, (g_dcm[num].rate * DCM_CNT_NUM) / 100);//lt:计数到第二个参数的值翻转电平
		break;
	case DCM2:
		tim5ConfigPWM(4, (g_dcm[num].rate * DCM_CNT_NUM) / 100);
		break;
	}
	g_dcm[num].sts = 1;
	tim5StartCounter();
}

/*
* 停止直流电机
* 停止PWM波输出，定时器对应通道的PWM比较值设置为0，即一直输出低电平
*/
void dcmStopDCM(u8 num)
{
	
	switch(num)
	{
	case DCM1:
		tim5ConfigPWM(3, 0);
		break;
	case DCM2:
		tim5ConfigPWM(4, 0);
		break;
	}
	g_dcm[num].sts = 0;
	tim5StartCounter();
}


/*
* 轻触开关初始化
* 轻触开关用于直流电机2的手动控制，每按一下直流电机2状态就改变
* 初始化电机控制引脚为外部中断脚
*/
void tchSWInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitStructure.GPIO_Pin = TCHSW_PIN_IN;//lt:PB1中断线
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

	 /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    NVIC_Init(&NVIC_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line1);
}


/*
* 轻触开关中断服务函数
* 用于控制直流电机2的启停，每按一下状态反转
*/
void tchSWIsr(void)
{
	pcFrame frame;
	frameInit((pcFrame*)&frame);
	if(g_dcm[DCM2].sts)
	{
		frame.cmd = DCM_START;
		dcmStopDCM(DCM2);
	}
	else
	{
		frame.cmd = DCM_STOP;
		dcmStartDCM(DCM2);
	}//lt?是否发送命令的逻辑反了
	frame.dev = 0x02;
	pcTxQueueAddFrame((pcFrame *)&frame);	
}//lt:从中断线1的中断函数中进入


