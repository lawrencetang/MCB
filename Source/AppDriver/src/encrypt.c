/*
encrypt.c

实现芯片程序加密
哪怕程序BIN文件被读出去，也不能运行。
简单的反汇编也无法破解本加密。

除芯片型号不对会死在本文件的代码以外，其他的都需要外部来做操作。
校验函数返回值为0时说明校验通过，否则会返回一个较为随机的数据。
这个随机数在外面的程序中可用来做循环控制，如果校验没通过，并不直接死机，而是运行不正常，给反汇给增加难度。

将设备ID、CRC校验结果、芯片序列号经过一定变换，存到FLASH中，在程序运行过程中校验这些数据，如果正确则程序正常运行，否则程序运行不正常。

定义原始数据（160位）：
32位设备ID：DID
CRC结果：CRC
96位序列号用三个数表示：SID1，SID2，SID3

定义存在FLASH中的数据（160位）：
32位设备ID：F_DID
CRC结果：F_CRC
96位序列号用三个数表示：F_SID1，F_SID2，F_SID3

加密算法如下（^表示异或，@表示同或）：
    1、 F_DID  = (DID  ^ SID1 ^ SID2 ^ KEY) @ SID3
	2、 F_CRC  = (CRC  ^ SID2 ^ KEY) @ SID3
	3、 F_SID1 = (SID1 ^ SID2 ^ KEY) @ DID
	4、 F_SID2 = (SID2 ^ SID3 ^ KEY) @ DID
	5、 F_SID3 = (SID3 ^ SID1 ^ KEY) @ DID


DID校验过程
	1、 读DID，读SID1
	2、 DIDA = DID ^ SID1 ^ SID2
	3、 获取F_DIDB = F_DID @ SID3，F_DIDB是在读FLASH的程序中运算得到
	4、 比较DIDA 和 F_DIDB，如果不等则死循环

CRC校验过程
	1、 计算程序和序列号的校验结果CRC，读SID2
	2、 CRCA = CRC ^ SID2
	3、 获取F_CRCB = F_CRC @ SID3 ^ KEY，F_CRCB是在读FLASH的程序中运算得到
	4、 读PA口的输入数据PA
	5、 返回 (CRCA ^ F_CRCB) * PA % 99999999，如果相同则返回0，否则可能是随机数


数据存储格式说明
    为了让数据存在FLASH中没有规律，对加密后的数进行分散再存储，共20字节的数据。
    A[n]为加密后的数据
    B[n]为A[n]均匀分散后的数据
    C[n]为B[n]置换后存储在FLASH中的数据
	定义FLASH读写数据变换
	读出过程：
	B[n] = ((C[(n+19)%20]<<4)&0xf0) | ((C[(n+1)%20]>>4)&0x0f)
	A[n] = B[(5 * n + n / 4) % 20]

	写入过程
	B[n] = A[(4 * n + n / 5) % 20]
	C[n] = ((B[(n+19)%20]<<4)&0xf0) | ((B[(n+1)%20]>>4)&0x0f)



yshy119
2016年1月6日
*/


#include "encrypt.h"
#include "stm32f10x_flash.h"


/*
*  程序读保护功能
*  一上电便禁止程序被读出
*/
void flashReadProtect(void)
{
	FLASH_Unlock();
	FLASH_ReadOutProtection(ENABLE);
	FLASH_Lock();
}



/*
* 读设备号DID
* 返回32位设备ID
*/
unsigned int readDeviceID(void)
{
	return DIVICE_ID;
}


/*
* 读序列号SID，分三段读
* id：段号 0 SID1，1 SID2，2 SID3
*/
unsigned int readSerialID(unsigned char id)
{ 
	if(id > 2)
	{
		return 0;
	}
	return *((unsigned int *)(ADDR_SERIALID + 4 * id));
}


/*
* 计算程序的CRC结果
* 计算程序代码段地址从ADDR_CRC_START到ADDR_CRC_END之间的CRC校验结果
* 把芯片序列号也加入到CRC计算中，使每个芯片的CRC计算结果都不一样
* 返回值：32位CRC校验结果
*/
unsigned int getProgramCRC(void)
{
	unsigned int l_addrProgram = ADDR_CRC_START;
	*((unsigned int *)(ADDR_CRC + 8)) = 0x00000001; 			//复位CRC单元
	while(l_addrProgram < ADDR_CRC_END)
	{
		*((unsigned int *)(ADDR_CRC)) = *(unsigned int *)(l_addrProgram);
		l_addrProgram = l_addrProgram + 4;
	}
	*((unsigned int *)(ADDR_CRC)) = readSerialID(0); 	//为保证每个芯片的CRC结果都不一样，把序列号加入到CRC计算中
	*((unsigned int *)(ADDR_CRC)) = readSerialID(1);
	*((unsigned int *)(ADDR_CRC)) = readSerialID(2);
	return *((unsigned int *)(ADDR_CRC));
}


/*
* 读FLASH
* 一次性将20字节全部读出，经过置换、去分散，得到的是加密后的数据
*/
void readFlashData(unsigned char *l_a)
{
	unsigned char i;	
	unsigned char l_b[20];
	unsigned char l_c[20];
	unsigned char l_d[20];
	FLASH_Unlock();
	for(i=0; i<20; )
	{
		*((unsigned int*)&l_d[i]) = *((unsigned int*)(ADDR_FLASH_ENCRYPT + i));	//读FLASH中的数据
		i = i + 4;
	}
	FLASH_Lock();
	for(i=0; i<20; i++)
	{
		l_c[i] = ((l_d[i]>>2)&0x3F) | ((l_d[(i+19)%20]<<6)&0xC0); 			//反移位 C[n] = s(D[n])
	}
	for(i=0; i<20; i++)
	{
		l_b[i] = ((l_c[(i+19)%20]<<4)&0xf0) | ((l_c[(i+1)%20]>>4)&0x0f); 	//逆置换 B[n] = f(C[n])
	}
	for(i=0; i<20; i++) 	 
	{
		l_a[i] = l_b[(i*5+i/4)%20];		//解分散 A[n] = B[(5*n)+n/4)%20]
	}
}


/*
* 写FLASH
* 一次性将20字节全部写入，输入数据是加密数据，经过分散、置换后存到FLASH中
*/
void writeFlashData(unsigned char *l_a)
{
	unsigned char i;	
	unsigned char l_b[20];
	unsigned char l_c[20];
	unsigned char l_d[20];
	for(i=0; i<20; i++) 	 
	{
		l_b[i] = l_a[(i*4+i/5)%20];		//分散 B[n] = A[(4*n)+n/5)%20]
	}
	for(i=0; i<20; i++)
	{
		l_c[i] = ((l_b[(i+19)%20]<<4)&0xf0) | ((l_b[(i+1)%20]>>4)&0x0f); 	//置换 C[n] = f(B[n])
	}
	for(i=0; i<20; i++)
	{
		l_d[i] = ((l_c[i]<<2)&0xFC) | ((l_c[(i+1)%20]>>6)&0x03); 			//移位 D[n] = s(C[n])
	}
	FLASH_Unlock();
	FLASH_ErasePage(ADDR_FLASH_ENCRYPT);
	for(i=0; i<20; )
	{
		FLASH_ProgramWord(ADDR_FLASH_ENCRYPT + i, *((unsigned int*)&l_d[i]));	//读FLASH中的数据
		i = i + 4;
	}
	FLASH_Lock();
}


/*
* 读FLASH中存储的加密后的数据
* 设备ID：4字节共32位，flag = 0。
* 程序CRC：4字节共32位，flag = 1。
* 序号号：12字节共96位，flag = 2, 3, 4。分三次返回。
*/
unsigned int readFlashEncryptData(unsigned char flag)
{
	unsigned int data = 0;
	unsigned char l_a[20];
	if(flag > 5)
	{
		return 0;	 
	}
	readFlashData((unsigned char*)&l_a[0]);
	data = *((unsigned int*)&l_a[flag*4]); 
	switch(flag)
	{
	case 0:		
		data = (data ^ readSerialID(2))^0xffffffff;
		break;
	case 1:
		data = (data ^ readSerialID(2))^0xffffffff;
		break;
	case 2:
	case 3:
	case 4:
		data = (data ^ readDeviceID())^0xffffffff;
		break;
	}
	data = data^ENCRYPT_KEY;
	return data;
}

/*
* 将加密后的数据写入到FLASH中
* 设备ID：4字节共32位，flag = 0。
* 程序CRC：4字节共32位，flag = 1。
* 序号号：12字节共96位，flag = 2, 3, 4。分三次写入。
*/
unsigned char writeFlashEncryptData(unsigned int data, unsigned char flag)
{
	unsigned char l_a[20];
	if(flag > 5)
	{
		return 0;	 
	}
	readFlashData((unsigned char*)&l_a[0]);
	*((unsigned int*)&l_a[flag*4]) = data;
	writeFlashData((unsigned char*)&l_a[0]);
	return 1;
}



/*
* 对芯片的设备号进行检查，型号相同的芯片设备号相同
* 芯片的设备号正确则返回0，否则返回值为一个较大的数据
*/
unsigned int deviceIDCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//如果没有定义加密，则直接返回0，即不检查直接通过
#endif 	

	unsigned int l_deviceID;
	unsigned int l_deviceIDFlash;
	l_deviceID = readDeviceID()^readSerialID(0)^readSerialID(1);	// DIDA = DID ^ SID1 ^ SID2
	l_deviceIDFlash = readFlashEncryptData(0); 		// F_DIDB 
	return (l_deviceID^l_deviceIDFlash);
}


/*
* 对程序段的CRC进行校验
* 如果CRC校验正确则返回0，否则返回值不确定（为0的可能性非常小）
*/
unsigned int crcCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//如果没有定义加密，则直接返回0，即不检查直接通过
#endif

   	unsigned int l_crcProgram;
	unsigned int l_crcFlash;
	unsigned int l_paInputData;
	l_crcProgram = getProgramCRC()^readSerialID(1);  	//CRCA = CRC ^ SID2
	l_crcFlash = readFlashEncryptData(1);				//F_CRCB
	l_paInputData = (*((unsigned int *)0x40010808)); 	//PA
	return ((l_crcProgram^l_crcFlash)*(l_paInputData))%99999999;	//两个结果求异或，异或值乘以当前PA口的输入状态，再用99999999取余，以便得到一个随机数值。
}

/*
* 对芯片序列号进行检查，每个芯片的序列号是唯一的
* 如果序列号正确则返回0，否则返回值不确定（为0的可能性非常小）
*/
unsigned int serialIDCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//如果没有定义加密，则直接返回0，即不检查直接通过
#endif

	unsigned int l_sidFlash1 = readFlashEncryptData(2); 			//F_SID1B
	unsigned int l_sidFlash2 = readFlashEncryptData(3);				//F_SID2B
	unsigned int l_sidFlash3 = readFlashEncryptData(4);	  			//F_SID3B
	unsigned int l_sidRead1 = readSerialID(0)^readSerialID(1);		//SID1A = SID1 ^ SID2
	unsigned int l_sidRead2 = readSerialID(1)^readSerialID(2);		//SID2A = SID2 ^ SID3
	unsigned int l_sidRead3 = readSerialID(2)^readSerialID(0);		//SID1A = SID3 ^ SID1
	unsigned int l_pbInputData = (*((unsigned int *)0x40010C08));	//PB
	return (((l_sidFlash1^l_sidRead1) + (l_sidFlash2^l_sidRead2) + (l_sidFlash3^l_sidRead3))*l_pbInputData)%99999999; 	//完全相等时才返回0
}





