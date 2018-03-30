/*
common.c
��ɢ���ܵ�ʵ��
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
/**************************************  ��Դ����  **************************************/
/****************************************************************************************/
/*
* �ϵ����Ϊʵ��һ�����ػ�����
* ��Դһ�ϵ�֮��ϵͳ��Ҫά������ĵ�Դ
* ϵͳ�ϵ�֮�󣬶�ʱ��ⰴ���������������ٴα�����ʱ��ϵͳ�ϵ�
* �� g_powerKEY �����水��״̬
*/
u8 g_powerKey = 1;
u8 g_controlShutdown = 0;
void powerON(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	    
  GPIO_InitStructure.GPIO_Pin = PIN_PWR_ON;//lt:PC8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PWR, &GPIO_InitStructure);

	POWER_ON;//lt:PC8��1

	GPIO_InitStructure.GPIO_Pin = PIN_PWR_DET;//lt:PC9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//lt:��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PWR, &GPIO_InitStructure);
	g_powerKey = 1;//lt:������Դ�Ͽ�����������ŵ�ѹΪ2.6V
}

/*
* ��Դ�������
* ����⵽��Դ���طſ�ʱ����¼g_powerKey = 0������Դ���طſ����ٴα�����ʱϵͳ�ϵ�
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
/***********************************  ���Ա۱պϼ��  ***********************************/
/****************************************************************************************/
/*
* ���Ա۱պϽӽ����س�ʼ��
* �� g_cswKey ���Ա�״̬��0 ��ʾδ�պϣ�1 ��ʾ�ѱպ�
* �����Ա۴�δ�պϵ��պ�״̬ʱ����������λ������һ��ָ��
*/
u8 g_cswKey = 1;
void cswInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	    
	GPIO_InitStructure.GPIO_Pin = CSW_PIN;//lt:PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//lt:��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CSW_GPIO, &GPIO_InitStructure);
	g_cswKey = 1;
}

/*
* ���Ա۽ӽ����ؼ��
* �� g_cswKey ���Ա�״̬��0 ��ʾδ�պϣ�1 ��ʾ�ѱպ�
* �����Ա۴ӶϿ�״̬�û�Ϊ�պ�״̬ʱ���Զ�����һ��ָ�����λ��  //lt:�ο���ѹ2.5V,����2.5vΪ1��С��2.5vΪ0
*/
void cswKeyDet(void)
{
	pcFrame frame;
	if(CSW_IN && (g_cswKey == 0))
	{
		frameInit((pcFrame *)&frame);//lt:����ָ��
		frame.cmd = READ_AUTO_TEST;  //lt:0x04
		frame.para = 0x01;
		frame.data[3] = 0x01;
		pcTxQueueAddFrame((pcFrame *)&frame);
	}
	g_cswKey = CSW_IN;
}


/****************************************************************************************/
/*************************************  LED�ƿ���  **************************************/
/****************************************************************************************/
/* LED���ų�ʼ�� */
void ledINIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	    
  GPIO_InitStructure.GPIO_Pin = PIN_LED;//lt:PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);

	LEDOFF();//lt:��λ�ر�led
}

/* ��LED */
void LEDON(void)
{    
	GPIO_ResetBits(GPIO_LED, PIN_LED);
}

/* �ر�LED */
void LEDOFF(void)
{
    GPIO_SetBits(GPIO_LED, PIN_LED);
}

/* LEDȡ�� */
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
/**************************************  ���ȿ���  **************************************/
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
	flag?FANON:FANOFF;//lt:PD3��1Ϊ���ȿ�����0Ϊ���ȹ�
}


/****************************************************************************************/
/**************************************  ��������  **************************************/
/****************************************************************************************/
/*
ͨ�Žӿڲ���ָ�Ӧ��ֱ�Ӱ�ԭ���ݷ���
����дӻ��Ļ���Ӧ�ø�ÿ���ӻ�������һ�ݲ���ָ�
*/
void CMDTestDeal(pcFrame frame)
{
	switch(frame.cmd)
	{																					 
	case COM_TEST:						//ͨ�Ų���//lt:0x00
		break;

	case FAN2_TEST:		   				//���Ȳ���//lt:0x01							
		frame.sts = STS_SUCCESS;//lt:0x00,����ָʾδ����?
		fanSet(frame.data[3]);
		break;

	case EEPROM_READ_TEST:		   		//��EEPROM
		iicRead((u16)((frame.data[0]<<8) | frame.data[1]), (u8*)&frame.data[2], 2);
		break;

	case EEPROM_WRITE_TEST:				//дEEPROM
		iicWrite((u16)((frame.data[0]<<8) | frame.data[1]), (u8*)&frame.data[2], 2);
		break;

	case READ_AUTO_TEST:				//���۱պ�״̬��Ѱ
		frame.data[3] = CSW_IN;//lt:����ָʾδ����,��λ���Ƿ��ܹ����յ�״̬����?
		break;

	default:
		frame.sts = STS_CMD_ERROR;
	}
	pcTxQueueAddFrame((pcFrame *)&frame);
}



/*
* ����״̬��Ѱ
*/
u8 boardStatusCMD(pcFrame frame)
{
	u32 data;
	switch(frame.cmd)
	{																					 
	case BOARD_STATUS:				//����״̬��Ѱ
		break;

	case READ_HDW_VERSION:		   	//Ӳ���汾��Ѱ							
		frame.sts = STS_SUCCESS;
		frame.para = 0x01;
		frame.data[0] = (HDW_VERSION >> 24) & 0xff;
		frame.data[1] = (HDW_VERSION >> 16) & 0xff;
		frame.data[2] = (HDW_VERSION >> 8) & 0xff;
		frame.data[3] = HDW_VERSION  & 0xff;
		break;

	case READ_SOFT_VERSION:		   	//����汾��Ѱ
		frame.sts = STS_SUCCESS;
		frame.para = 0x01;
		frame.data[0] = (SOFT_VERSION >> 24) & 0xff;
		frame.data[1] = (SOFT_VERSION >> 16) & 0xff;
		frame.data[2] = (SOFT_VERSION >> 8) & 0xff;
		frame.data[3] = SOFT_VERSION  & 0xff;
		break;

	case SET_PRODUCT_KIND:			//���ò�Ʒ����
		break;
		
	case READ_SERIAL_ID:			//��оƬ���к�
		data = readSerialID(frame.dev - 1);
		frame.para = 0x01;
		frame.data[0] = (data >> 24) & 0xff;
		frame.data[1] = (data >> 16) & 0xff;
		frame.data[2] = (data >> 8) & 0xff;
		frame.data[3] = data  & 0xff;
		break;	
	
  case SHUTDOWN_MAINBOARD:   //�ػ�
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
* ΢�뼶��ʱ����
*/
void delayUS(u32 dly)
{
	dly = dly * 4; 	/* ������Ϊ0.95us */
	while(dly--) ;
}

/*
* ���뼶��ʱ����
*/
void delayMS(u32 dly)
{
	dly = dly * 5400;	/* ������Ϊ1ms */
	while(dly--) ;
}






