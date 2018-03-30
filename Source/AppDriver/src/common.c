/*
common.c
零散功能的实现
*/
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "common.h"
#include "protocal.h"
#include "pcComm.h"
#include "eeprom.h"
#include "encrypt.h"
#include "task.h"


/****************************************************************************************/
/**************************************  电源控制  **************************************/
/****************************************************************************************/
/*
* 上电程序，为实现一键开关机功能
* 电源一上电之后，系统需要维持自身的电源
* 系统上电之后，定时检测按键，当按键弹起并再次被按下时，系统断电
* 用 g_powerKEY 来保存按键状态
*/
u8 g_powerKey = 1;
u8 g_controlShutdown = 0;
void powerON(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	    
  GPIO_InitStructure.GPIO_Pin = PIN_PWR_ON;//lt:PC8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PWR, &GPIO_InitStructure);

	POWER_ON;//lt:PC8置1

	GPIO_InitStructure.GPIO_Pin = PIN_PWR_DET;//lt:PC9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//lt:浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PWR, &GPIO_InitStructure);
	g_powerKey = 1;//lt:开启电源断开按键后该引脚电压为2.6V
}

/*
* 电源按键检测
* 当检测到电源开关放开时，记录g_powerKey = 0，当电源开关放开后再次被按下时系统断电
*/
void powerKeyDet(void)
{
	/*if(POWER_DET)  //lt:PC9 = 1
	{
		g_powerKey = 0;
	}
	else if(g_powerKey == 0)
	{
		POWER_OFF;  //lt:PC8 = 0
	}*/
	//ltg
	if (g_controlShutdown== 1)
	{
		OSTimeDlyHMSM(0,0,50,0);
		POWER_OFF;
	}
}


/****************************************************************************************/
/***********************************  测试臂闭合检测  ***********************************/
/****************************************************************************************/
/*
* 测试臂闭合接近开关初始化
* 用 g_cswKey 测试臂状态，0 表示未闭合，1 表示已闭合
* 当测试臂从未闭合到闭合状态时，主动向上位机发送一条指令
*/
u8 g_cswKey = 1;
void cswInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	    
	GPIO_InitStructure.GPIO_Pin = CSW_PIN;//lt:PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//lt:浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CSW_GPIO, &GPIO_InitStructure);
	g_cswKey = 1;
}

/*
* 测试臂接近开关检测
* 用 g_cswKey 测试臂状态，0 表示未闭合，1 表示已闭合
* 当测试臂从断开状态置换为闭合状态时，自动发送一条指令给上位机  //lt:参考电压2.5V,大于2.5v为1，小于2.5v为0
*/
void cswKeyDet(void)
{
	pcFrame frame;
	if(CSW_IN && (g_cswKey == 0))
	{
		frameInit((pcFrame *)&frame);//lt:上行指令
		frame.cmd = READ_AUTO_TEST;  //lt:0x04
		frame.para = 0x01;
		frame.data[3] = 0x01;
		pcTxQueueAddFrame((pcFrame *)&frame);
	}
	g_cswKey = CSW_IN;
}


/****************************************************************************************/
/*************************************  LED灯控制  **************************************/
/****************************************************************************************/
/* LED引脚初始化 */
void ledINIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	    
  GPIO_InitStructure.GPIO_Pin = PIN_LED;//lt:PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);

	LEDOFF();//lt:置位关闭led
}

/* 打开LED */
void LEDON(void)
{    
	GPIO_ResetBits(GPIO_LED, PIN_LED);
}

/* 关闭LED */
void LEDOFF(void)
{
    GPIO_SetBits(GPIO_LED, PIN_LED);
}

/* LED取反 */
void LEDREV(void)
{
	if(GPIO_ReadInputDataBit(GPIO_LED, PIN_LED))
	{	
		LEDON();
	}
	else
	{
		LEDOFF();
	}
}


/****************************************************************************************/
/**************************************  风扇控制  **************************************/
/****************************************************************************************/
void fanInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	    
    GPIO_InitStructure.GPIO_Pin = PIN_FAN;//lt:PD3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_FAN, &GPIO_InitStructure);

	FANOFF;
}

void fanSet(u8 flag)
{
	flag?FANON:FANOFF;//lt:PD3置1为风扇开，置0为风扇关
}


/****************************************************************************************/
/**************************************  其他函数  **************************************/
/****************************************************************************************/
/*
通信接口测试指令，应该直接按原数据返回
如果有从机的话，应该给每个从机都发送一份测试指令。
*/
void CMDTestDeal(pcFrame frame)
{
	switch(frame.cmd)
	{																					 
	case COM_TEST:						//通信测试//lt:0x00
		break;

	case FAN2_TEST:		   				//风扇测试//lt:0x01							
		frame.sts = STS_SUCCESS;//lt:0x00,参数指示未更改?
		fanSet(frame.data[3]);
		break;

	case EEPROM_READ_TEST:		   		//读EEPROM
		iicRead((u16)((frame.data[0]<<8) | frame.data[1]), (u8*)&frame.data[2], 2);
		break;

	case EEPROM_WRITE_TEST:				//写EEPROM
		iicWrite((u16)((frame.data[0]<<8) | frame.data[1]), (u8*)&frame.data[2], 2);
		break;

	case READ_AUTO_TEST:				//检测臂闭合状态查寻
		frame.data[3] = CSW_IN;//lt:参数指示未更改,上位机是否能够接收到状态数据?
		break;

	default:
		frame.sts = STS_CMD_ERROR;
	}
	pcTxQueueAddFrame((pcFrame *)&frame);
}



/*
* 板子状态查寻
*/
u8 boardStatusCMD(pcFrame frame)
{
	u32 data;
	switch(frame.cmd)
	{																					 
	case BOARD_STATUS:				//板子状态查寻
		break;

	case READ_HDW_VERSION:		   	//硬件版本查寻							
		frame.sts = STS_SUCCESS;
		frame.para = 0x01;
		frame.data[0] = (HDW_VERSION >> 24) & 0xff;
		frame.data[1] = (HDW_VERSION >> 16) & 0xff;
		frame.data[2] = (HDW_VERSION >> 8) & 0xff;
		frame.data[3] = HDW_VERSION  & 0xff;
		break;

	case READ_SOFT_VERSION:		   	//软件版本查寻
		frame.sts = STS_SUCCESS;
		frame.para = 0x01;
		frame.data[0] = (SOFT_VERSION >> 24) & 0xff;
		frame.data[1] = (SOFT_VERSION >> 16) & 0xff;
		frame.data[2] = (SOFT_VERSION >> 8) & 0xff;
		frame.data[3] = SOFT_VERSION  & 0xff;
		break;

	case SET_PRODUCT_KIND:			//设置产品类型
		break;
		
	case READ_SERIAL_ID:			//读芯片序列号
		data = readSerialID(frame.dev - 1);
		frame.para = 0x01;
		frame.data[0] = (data >> 24) & 0xff;
		frame.data[1] = (data >> 16) & 0xff;
		frame.data[2] = (data >> 8) & 0xff;
		frame.data[3] = data  & 0xff;
		break;	
	
  case SHUTDOWN_MAINBOARD:   //关机
		g_controlShutdown = 1;
		break;			

	default:
		frame.sts = STS_CMD_ERROR;
	}
	pcTxQueueAddFrame((pcFrame *)&frame);
	return STS_SUCCESS;
}


void encryptCMDDeal(pcFrame frame)
{
	unsigned int data;
	data = (frame.data[0]<<24) | (frame.data[1]<<16) | (frame.data[2]<<8) | (frame.data[3]); 
	switch(frame.cmd)
	{																					 
	case READ_DEVICEID:					//
		data = frame.dev?readFlashEncryptData(0):readDeviceID();
		break;
	case READ_PROM_CRC:					//
		data = frame.dev?readFlashEncryptData(1):getProgramCRC();
		break;
	case READ_SERIALID1:				//
		data = frame.dev?readFlashEncryptData(2):readSerialID(0);
		break;
	case READ_SERIALID2:				//
		data = frame.dev?readFlashEncryptData(3):readSerialID(1);
		break;
	case READ_SERIALID3:				//
		data = frame.dev?readFlashEncryptData(4):readSerialID(2);
		break;
	case WRITE_DEVICEID:				//
		writeFlashEncryptData(data, 0);
		break;
	case WRITE_PROM_CRC:				//
		writeFlashEncryptData(data, 1);
		break;
	case WRITE_SERIALID1:				//
		writeFlashEncryptData(data, 2);
		break;
	case WRITE_SERIALID2:				//
		writeFlashEncryptData(data, 3);
		break;
	case WRITE_SERIALID3:				//
		writeFlashEncryptData(data, 4);
		break;
	default:
		frame.sts = STS_CMD_ERROR;
	}
	frame.para = 0x01;
	frame.data[0] = (data>>24) & 0xff;
	frame.data[1] = (data>>16) & 0xff;
	frame.data[2] = (data>>8 ) & 0xff;
	frame.data[3] =  data & 0xff;
	pcTxQueueAddFrame((pcFrame *)&frame);
}



/*
* 微秒级延时函数
*/
void delayUS(u32 dly)
{
	dly = dly * 4; 	/* 经测试为0.95us */
	while(dly--) ;
}

/*
* 毫秒级延时函数
*/
void delayMS(u32 dly)
{
	dly = dly * 5400;	/* 经测试为1ms */
	while(dly--) ;
}






