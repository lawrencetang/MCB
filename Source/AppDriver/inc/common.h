/*
2012.01.12
*/


#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f10x_gpio.h"
#include "frame.h"

/* 硬件版本，时间 */
#define HDW_VERSION 	0x20151216

/* 软件版本，时间 */
#define SOFT_VERSION 	0x20160413

/* 芯片序列号地址，96位唯一ID */
#define ADDR_SERIALID 		0x1FFFF7E8 		


/****************************************************************************************/
/**************************************  电源控制  **************************************/
/****************************************************************************************/
/* 电源控制与关机检测 */
#define GPIO_PWR 		GPIOC
#define PIN_PWR_ON  	GPIO_Pin_8 		//PC8
#define PIN_PWR_DET  	GPIO_Pin_9 		//PC9
#define POWER_ON 		GPIO_SetBits(GPIO_PWR, PIN_PWR_ON)//lt:置位PC8
#define POWER_OFF 		GPIO_ResetBits(GPIO_PWR, PIN_PWR_ON)//lt:复位PC8
#define POWER_DET 		GPIO_ReadInputDataBit(GPIO_PWR, PIN_PWR_DET)//lt:读PC9位

/*
* 上电程序，为实现一键开关机功能
* 电源一上电之后，系统需要维持自身的电源
* 系统上电之后，定时检测按键，当按键弹起并再次被按下时，系统断电
* 用 g_powerKEY 来保存按键状态
*/
void powerON(void);

/*
* 电源按键检测
* 当检测到电源开关放开时，记录g_powerKey = 0，当电源开关放开后再次被按下时系统断电
*/
void powerKeyDet(void);



/****************************************************************************************/
/***********************************  测试臂闭合检测  ***********************************/
/****************************************************************************************/
/* 测试臂闭合接近开关 */
#define CSW_GPIO 		GPIOA
#define CSW_PIN 		GPIO_Pin_1		//PA1 
#define CSW_IN 			(GPIO_ReadInputDataBit(CSW_GPIO, CSW_PIN)^0x01)	  //测试臂闭合时输出1//lt:按位异或

/*
* 测试臂闭合接近开关初始化
* 用 g_cswKey 测试臂状态，0 表示未闭合，1 表示已闭合
* 当测试臂从未闭合到闭合状态时，主动向上位机发送一条指令
*/
void cswInit(void);

/*
* 测试臂接近开关检测
* 用 g_cswKey 测试臂状态，0 表示未闭合，1 表示已闭合
* 当测试臂从断开状态置换为闭合状态时，自动发送一条指令给上位机
*/
void cswKeyDet(void);




/****************************************************************************************/
/*************************************  LED灯控制  **************************************/
/****************************************************************************************/
/* LED灯 */
#define GPIO_LED 		GPIOC
#define PIN_LED 		GPIO_Pin_11		//PC11 
 
/* LED引脚初始化 */
void ledINIT(void);

/* 打开LED */
void LEDON(void);

/* 关闭LED */
void LEDOFF(void);

/* LED取反 */
void LEDREV(void);



/****************************************************************************************/
/**************************************  风扇控制  **************************************/
/****************************************************************************************/
/* 风扇控制，加热片散热用 */
#define GPIO_FAN 		GPIOD
#define PIN_FAN 		GPIO_Pin_3		//PD3  

#define FANON 		GPIO_SetBits(GPIO_FAN, PIN_FAN)
#define FANOFF 		GPIO_ResetBits(GPIO_FAN, PIN_FAN)
void fanInit(void);
void fanSet(u8 flag);




/****************************************************************************************/
/**************************************  其他函数  **************************************/
/****************************************************************************************/
/*
通信接口测试指令，应该直接按原数据返回
如果有从机的话，应该给每个从机都发送一份测试指令。
*/
void CMDTestDeal(pcFrame frame);



/*
* 板子状态查寻
*/
u8 boardStatusCMD(pcFrame frame);


/*
*  程序读保护功能
*  一上电便禁止程序被读出
*/
void flashReadProtect(void);

/*
* 加密指令处理
*/
void encryptCMDDeal(pcFrame frame);

/*
* 微秒级延时函数
*/
void delayUS(u32 dly);

/*
* 毫秒级延时函数
*/
void delayMS(u32 dly);

#endif




