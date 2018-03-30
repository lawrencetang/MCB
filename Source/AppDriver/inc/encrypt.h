

#ifndef _ENCRYPT_CHIP_H
#define _ENCRYPT_CHIP_H

/* 定义是否对芯片进行加密 */
#define ENCRYPT_CHIP 		1

/* 芯片内部寄存器地址 */
#define ADDR_DEVECEID 		0xE0042000 		//设备ID地址，32位芯片型号
#define ADDR_SERIALID 		0x1FFFF7E8 		//芯片序列号地址，96位唯一ID
#define ADDR_CRC 			0x40023000 		//CRC-32数据读写地址，32位数据

/* 存放加密数据的地址，需要160位共20字节的连续空间 */
#define ADDR_FLASH_ENCRYPT 	0x08010000 		//设备ID地址，32位芯片型号，存放在64K位置后


#define ADDR_CRC_START 		0x08000000 		//程序CRC校验起始地址
#define ADDR_CRC_END 		0x08010000 		//程序CRC校验结束地址，不包含该地址，校验64K程序


#define DIVICE_ID 			0x59534859 		//定义设备号
#define ENCRYPT_KEY 		0x3579ABCD 		//自定义密钥


/*
*  程序读保护功能
*  一上电便禁止程序被读出
*/
void flashReadProtect(void);


/*
* 读设备号DID
* 返回32位设备ID
*/
unsigned int readDeviceID(void);

/*
* 读序列号SID，分三段读
* id：段号 0 SID1，1 SID2，2 SID3
*/
unsigned int readSerialID(unsigned char id);


/*
* 计算程序的CRC结果
* 计算程序代码段地址从ADDR_CRC_START到ADDR_CRC_END之间的CRC校验结果
* 把芯片序列号也加入到CRC计算中，使每个芯片的CRC计算结果都不一样
* 返回值：32位CRC校验结果
*/
unsigned int getProgramCRC(void);


/*
* 读FLASH
* 一次性将20字节全部读出，经过置换、去分散，得到的是加密后的数据
*/
void readFlashData(unsigned char *l_a);


/*
* 写FLASH
* 一次性将20字节全部写入，输入数据是加密数据，经过分散、置换后存到FLASH中
*/
void writeFlashData(unsigned char *l_a);


/*
* 读FLASH中存储的加密后的数据
* 设备ID：4字节共32位，flag = 0。
* 程序CRC：4字节共32位，flag = 1。
* 序号号：12字节共96位，flag = 2, 3, 4。分三次返回。
*/
unsigned int readFlashEncryptData(unsigned char flag);


/*
* 将加密后的数据写入到FLASH中
* 设备ID：4字节共32位，flag = 0。
* 程序CRC：4字节共32位，flag = 1。
* 序号号：12字节共96位，flag = 2, 3, 4。分三次写入。
*/
unsigned char writeFlashEncryptData(unsigned int data, unsigned char flag);


/*
* 对芯片的设备号进行检查，型号相同的芯片设备号相同
* 芯片的设备号正确则返回0，否则返回值为一个较大的数据
*/
unsigned int deviceIDCheck(void);


/*
* 对程序段的CRC进行校验
* 如果CRC校验正确则返回0，否则返回值不确定（为0的可能性非常小）
*/
unsigned int crcCheck(void);


/*
* 对芯片序列号进行检查，每个芯片的序列号是唯一的
* 如果序列号正确则返回0，否则返回值不确定（为0的可能性非常小）
*/
unsigned int serialIDCheck(void);


#endif


