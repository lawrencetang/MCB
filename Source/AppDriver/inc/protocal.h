/*
2015.05.15
*/

#ifndef _MY_PROTOCAL_H
#define _MY_PROTOCAL_H

/****************************************************/
/******************** 帧标识定义 ********************/
/****************************************************/
#define DOWN_FRAME 					0xC0
#define UP_FRAME 						0xB0


/****************************************************/
/******************** 定义命令字 ********************/
/****************************************************/
/**************测试指令**************/
#define COM_TEST 						0x00 /* 通信测试 */
#define FAN2_TEST						0x01 /* 风扇2测试 */
#define EEPROM_READ_TEST		0x02 /* 写EEPROM */
#define EEPROM_WRITE_TEST		0x03 /* 读EEPROM */
#define READ_AUTO_TEST			0x04 /* 查寻自动测试开关状态 */


/**************查寻指令**************/
#define BOARD_STATUS 				0x70 /* 查寻主板状态 */
#define READ_HDW_VERSION		0x71 /* 查寻硬件版本 */
#define READ_SOFT_VERSION		0x72 /* 查寻软件版本 */
#define SET_PRODUCT_KIND		0x74 /* 设置产品类型 */
#define READ_SERIAL_ID  		0x75 /* 读取芯片序列号 */
#define SHUTDOWN_MAINBOARD  0x76 /* 关机 */


/**************步进电机指令**************/
#define SM_CMD 							0x80 /* 步进电机操作相关 */
#define SM_CHECK 						0x80 /* 步进电机故障查寻 */
#define SM_START  					0x81 /* 步进电机转动到指定角度 */
#define SM_STOP  						0x82 /* 强制停止步进电机 */
#define SM_READAPOS					0x83 /* 读取步进电机当前角度 */
#define SM_READZERO  				0x84 /* 读取步进电机逻辑零点 */
#define SM_SETAERO  				0x85 /* 设置步进电机逻辑零点 */

/**************直流电机指令**************/
#define DCM_CMD 						0x90 /* 直流电机操作相关 */
#define DCM_CHECK 					0x90 /* 直流电机故障查寻 */
#define DCM_START  					0x91 /* 启动直流电机 */
#define DCM_STOP  					0x92 /* 停止直流电机 */


/**************恒温控制指令**************/
#define TEM_CMD 						0xA0 /* 恒温操作相关 */
#define TEM_CHECK 					0xA0 /* 恒温故障查寻 */
#define TEM_START  					0xA1 /* 启动恒温 */
#define TEM_STOP  					0xA2 /* 停止恒温 */
#define TEM_READ  					0xA3 /* 读当前温度 */




/****************************************************/
/******************* 操作结果状态 *******************/
/****************************************************/
/**************上位机指令操作结果**************/
#define STS_SUCCESS	 				0x00 /* 成功 */
#define STS_FAILED	 				0x01 /* 失败 */
#define STS_BUZY	 					0x02 /* 繁忙 */
#define STS_CMD_ERROR 			0x03 /* 指令错误 */
#define STS_DEV_ERROR 			0x04 /* 设备编号错误 */
#define STS_DATA_ERROR 			0x05 /* 参数错误 */
#define STS_FAULT 					0x06 /* 设备故障 */

#define NO_ERROR 						0x00 /* 没有错误 */




/**************加密相关指令**************/
#define ENCRYPT_CMD 				0xD0

#define READ_DEVICEID 			0xD1
#define READ_PROM_CRC 			0xD2
#define READ_SERIALID1 			0xD3
#define READ_SERIALID2 			0xD4
#define READ_SERIALID3 			0xD5

#define WRITE_DEVICEID 			0xD6
#define WRITE_PROM_CRC 			0xD7
#define WRITE_SERIALID1 		0xD8
#define WRITE_SERIALID2 		0xD9
#define WRITE_SERIALID3 		0xDA
 

#endif


