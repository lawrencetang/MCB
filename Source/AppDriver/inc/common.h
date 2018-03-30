/*
2012.01.12
*/


#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f10x_gpio.h"
#include "frame.h"

/* Ӳ���汾��ʱ�� */
#define HDW_VERSION 	0x20151216

/* �����汾��ʱ�� */
#define SOFT_VERSION 	0x20160413

/* оƬ���кŵ�ַ��96λΨһID */
#define ADDR_SERIALID 		0x1FFFF7E8 		


/****************************************************************************************/
/**************************************  ��Դ����  **************************************/
/****************************************************************************************/
/* ��Դ������ػ���� */
#define GPIO_PWR 		GPIOC
#define PIN_PWR_ON  	GPIO_Pin_8 		//PC8
#define PIN_PWR_DET  	GPIO_Pin_9 		//PC9
#define POWER_ON 		GPIO_SetBits(GPIO_PWR, PIN_PWR_ON)//lt:��λPC8
#define POWER_OFF 		GPIO_ResetBits(GPIO_PWR, PIN_PWR_ON)//lt:��λPC8
#define POWER_DET 		GPIO_ReadInputDataBit(GPIO_PWR, PIN_PWR_DET)//lt:��PC9λ

/*
* �ϵ����Ϊʵ��һ�����ػ�����
* ��Դһ�ϵ�֮��ϵͳ��Ҫά�������ĵ�Դ
* ϵͳ�ϵ�֮�󣬶�ʱ��ⰴ���������������ٴα�����ʱ��ϵͳ�ϵ�
* �� g_powerKEY �����水��״̬
*/
void powerON(void);

/*
* ��Դ�������
* ����⵽��Դ���طſ�ʱ����¼g_powerKey = 0������Դ���طſ����ٴα�����ʱϵͳ�ϵ�
*/
void powerKeyDet(void);



/****************************************************************************************/
/***********************************  ���Ա۱պϼ��  ***********************************/
/****************************************************************************************/
/* ���Ա۱պϽӽ����� */
#define CSW_GPIO 		GPIOA
#define CSW_PIN 		GPIO_Pin_1		//PA1 
#define CSW_IN 			(GPIO_ReadInputDataBit(CSW_GPIO, CSW_PIN)^0x01)	  //���Ա۱պ�ʱ���1//lt:��λ���

/*
* ���Ա۱պϽӽ����س�ʼ��
* �� g_cswKey ���Ա�״̬��0 ��ʾδ�պϣ�1 ��ʾ�ѱպ�
* �����Ա۴�δ�պϵ��պ�״̬ʱ����������λ������һ��ָ��
*/
void cswInit(void);

/*
* ���Ա۽ӽ����ؼ��
* �� g_cswKey ���Ա�״̬��0 ��ʾδ�պϣ�1 ��ʾ�ѱպ�
* �����Ա۴ӶϿ�״̬�û�Ϊ�պ�״̬ʱ���Զ�����һ��ָ�����λ��
*/
void cswKeyDet(void);




/****************************************************************************************/
/*************************************  LED�ƿ���  **************************************/
/****************************************************************************************/
/* LED�� */
#define GPIO_LED 		GPIOC
#define PIN_LED 		GPIO_Pin_11		//PC11 
 
/* LED���ų�ʼ�� */
void ledINIT(void);

/* ��LED */
void LEDON(void);

/* �ر�LED */
void LEDOFF(void);

/* LEDȡ�� */
void LEDREV(void);



/****************************************************************************************/
/**************************************  ���ȿ���  **************************************/
/****************************************************************************************/
/* ���ȿ��ƣ�����Ƭɢ���� */
#define GPIO_FAN 		GPIOD
#define PIN_FAN 		GPIO_Pin_3		//PD3  

#define FANON 		GPIO_SetBits(GPIO_FAN, PIN_FAN)
#define FANOFF 		GPIO_ResetBits(GPIO_FAN, PIN_FAN)
void fanInit(void);
void fanSet(u8 flag);




/****************************************************************************************/
/**************************************  ��������  **************************************/
/****************************************************************************************/
/*
ͨ�Žӿڲ���ָ�Ӧ��ֱ�Ӱ�ԭ���ݷ���
����дӻ��Ļ���Ӧ�ø�ÿ���ӻ�������һ�ݲ���ָ�
*/
void CMDTestDeal(pcFrame frame);



/*
* ����״̬��Ѱ
*/
u8 boardStatusCMD(pcFrame frame);


/*
*  �������������
*  һ�ϵ���ֹ���򱻶���
*/
void flashReadProtect(void);

/*
* ����ָ���
*/
void encryptCMDDeal(pcFrame frame);

/*
* ΢�뼶��ʱ����
*/
void delayUS(u32 dly);

/*
* ���뼶��ʱ����
*/
void delayMS(u32 dly);

#endif



