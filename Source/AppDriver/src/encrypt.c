/*
encrypt.c

ʵ��оƬ�������
���³���BIN�ļ�������ȥ��Ҳ�������С�
�򵥵ķ����Ҳ�޷��ƽⱾ���ܡ�

��оƬ�ͺŲ��Ի����ڱ��ļ��Ĵ������⣬�����Ķ���Ҫ�ⲿ����������
У�麯������ֵΪ0ʱ˵��У��ͨ��������᷵��һ����Ϊ��������ݡ�
��������������ĳ����п�������ѭ�����ƣ����У��ûͨ��������ֱ���������������в�������������������Ѷȡ�

���豸ID��CRCУ������оƬ���кž���һ���任���浽FLASH�У��ڳ������й�����У����Щ���ݣ������ȷ������������У�����������в�������

����ԭʼ���ݣ�160λ����
32λ�豸ID��DID
CRC�����CRC
96λ���к�����������ʾ��SID1��SID2��SID3

�������FLASH�е����ݣ�160λ����
32λ�豸ID��F_DID
CRC�����F_CRC
96λ���к�����������ʾ��F_SID1��F_SID2��F_SID3

�����㷨���£�^��ʾ���@��ʾͬ�򣩣�
    1�� F_DID  = (DID  ^ SID1 ^ SID2 ^ KEY) @ SID3
	2�� F_CRC  = (CRC  ^ SID2 ^ KEY) @ SID3
	3�� F_SID1 = (SID1 ^ SID2 ^ KEY) @ DID
	4�� F_SID2 = (SID2 ^ SID3 ^ KEY) @ DID
	5�� F_SID3 = (SID3 ^ SID1 ^ KEY) @ DID


DIDУ�����
	1�� ��DID����SID1
	2�� DIDA = DID ^ SID1 ^ SID2
	3�� ��ȡF_DIDB = F_DID @ SID3��F_DIDB���ڶ�FLASH�ĳ���������õ�
	4�� �Ƚ�DIDA �� F_DIDB�������������ѭ��

CRCУ�����
	1�� �����������кŵ�У����CRC����SID2
	2�� CRCA = CRC ^ SID2
	3�� ��ȡF_CRCB = F_CRC @ SID3 ^ KEY��F_CRCB���ڶ�FLASH�ĳ���������õ�
	4�� ��PA�ڵ���������PA
	5�� ���� (CRCA ^ F_CRCB) * PA % 99999999�������ͬ�򷵻�0����������������


���ݴ洢��ʽ˵��
    Ϊ�������ݴ���FLASH��û�й��ɣ��Լ��ܺ�������з�ɢ�ٴ洢����20�ֽڵ����ݡ�
    A[n]Ϊ���ܺ������
    B[n]ΪA[n]���ȷ�ɢ�������
    C[n]ΪB[n]�û���洢��FLASH�е�����
	����FLASH��д���ݱ任
	�������̣�
	B[n] = ((C[(n+19)%20]<<4)&0xf0) | ((C[(n+1)%20]>>4)&0x0f)
	A[n] = B[(5 * n + n / 4) % 20]

	д�����
	B[n] = A[(4 * n + n / 5) % 20]
	C[n] = ((B[(n+19)%20]<<4)&0xf0) | ((B[(n+1)%20]>>4)&0x0f)



yshy119
2016��1��6��
*/


#include "encrypt.h"
#include "stm32f10x_flash.h"


/*
*  �������������
*  һ�ϵ���ֹ���򱻶���
*/
void flashReadProtect(void)
{
	FLASH_Unlock();
	FLASH_ReadOutProtection(ENABLE);
	FLASH_Lock();
}



/*
* ���豸��DID
* ����32λ�豸ID
*/
unsigned int readDeviceID(void)
{
	return DIVICE_ID;
}


/*
* �����к�SID�������ζ�
* id���κ� 0 SID1��1 SID2��2 SID3
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
* ��������CRC���
* ����������ε�ַ��ADDR_CRC_START��ADDR_CRC_END֮���CRCУ����
* ��оƬ���к�Ҳ���뵽CRC�����У�ʹÿ��оƬ��CRC����������һ��
* ����ֵ��32λCRCУ����
*/
unsigned int getProgramCRC(void)
{
	unsigned int l_addrProgram = ADDR_CRC_START;
	*((unsigned int *)(ADDR_CRC + 8)) = 0x00000001; 			//��λCRC��Ԫ
	while(l_addrProgram < ADDR_CRC_END)
	{
		*((unsigned int *)(ADDR_CRC)) = *(unsigned int *)(l_addrProgram);
		l_addrProgram = l_addrProgram + 4;
	}
	*((unsigned int *)(ADDR_CRC)) = readSerialID(0); 	//Ϊ��֤ÿ��оƬ��CRC�������һ���������кż��뵽CRC������
	*((unsigned int *)(ADDR_CRC)) = readSerialID(1);
	*((unsigned int *)(ADDR_CRC)) = readSerialID(2);
	return *((unsigned int *)(ADDR_CRC));
}


/*
* ��FLASH
* һ���Խ�20�ֽ�ȫ�������������û���ȥ��ɢ���õ����Ǽ��ܺ������
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
		*((unsigned int*)&l_d[i]) = *((unsigned int*)(ADDR_FLASH_ENCRYPT + i));	//��FLASH�е�����
		i = i + 4;
	}
	FLASH_Lock();
	for(i=0; i<20; i++)
	{
		l_c[i] = ((l_d[i]>>2)&0x3F) | ((l_d[(i+19)%20]<<6)&0xC0); 			//����λ C[n] = s(D[n])
	}
	for(i=0; i<20; i++)
	{
		l_b[i] = ((l_c[(i+19)%20]<<4)&0xf0) | ((l_c[(i+1)%20]>>4)&0x0f); 	//���û� B[n] = f(C[n])
	}
	for(i=0; i<20; i++) 	 
	{
		l_a[i] = l_b[(i*5+i/4)%20];		//���ɢ A[n] = B[(5*n)+n/4)%20]
	}
}


/*
* дFLASH
* һ���Խ�20�ֽ�ȫ��д�룬���������Ǽ������ݣ�������ɢ���û���浽FLASH��
*/
void writeFlashData(unsigned char *l_a)
{
	unsigned char i;	
	unsigned char l_b[20];
	unsigned char l_c[20];
	unsigned char l_d[20];
	for(i=0; i<20; i++) 	 
	{
		l_b[i] = l_a[(i*4+i/5)%20];		//��ɢ B[n] = A[(4*n)+n/5)%20]
	}
	for(i=0; i<20; i++)
	{
		l_c[i] = ((l_b[(i+19)%20]<<4)&0xf0) | ((l_b[(i+1)%20]>>4)&0x0f); 	//�û� C[n] = f(B[n])
	}
	for(i=0; i<20; i++)
	{
		l_d[i] = ((l_c[i]<<2)&0xFC) | ((l_c[(i+1)%20]>>6)&0x03); 			//��λ D[n] = s(C[n])
	}
	FLASH_Unlock();
	FLASH_ErasePage(ADDR_FLASH_ENCRYPT);
	for(i=0; i<20; )
	{
		FLASH_ProgramWord(ADDR_FLASH_ENCRYPT + i, *((unsigned int*)&l_d[i]));	//��FLASH�е�����
		i = i + 4;
	}
	FLASH_Lock();
}


/*
* ��FLASH�д洢�ļ��ܺ������
* �豸ID��4�ֽڹ�32λ��flag = 0��
* ����CRC��4�ֽڹ�32λ��flag = 1��
* ��źţ�12�ֽڹ�96λ��flag = 2, 3, 4�������η��ء�
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
* �����ܺ������д�뵽FLASH��
* �豸ID��4�ֽڹ�32λ��flag = 0��
* ����CRC��4�ֽڹ�32λ��flag = 1��
* ��źţ�12�ֽڹ�96λ��flag = 2, 3, 4��������д�롣
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
* ��оƬ���豸�Ž��м�飬�ͺ���ͬ��оƬ�豸����ͬ
* оƬ���豸����ȷ�򷵻�0�����򷵻�ֵΪһ���ϴ������
*/
unsigned int deviceIDCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//���û�ж�����ܣ���ֱ�ӷ���0���������ֱ��ͨ��
#endif 	

	unsigned int l_deviceID;
	unsigned int l_deviceIDFlash;
	l_deviceID = readDeviceID()^readSerialID(0)^readSerialID(1);	// DIDA = DID ^ SID1 ^ SID2
	l_deviceIDFlash = readFlashEncryptData(0); 		// F_DIDB 
	return (l_deviceID^l_deviceIDFlash);
}


/*
* �Գ���ε�CRC����У��
* ���CRCУ����ȷ�򷵻�0�����򷵻�ֵ��ȷ����Ϊ0�Ŀ����Էǳ�С��
*/
unsigned int crcCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//���û�ж�����ܣ���ֱ�ӷ���0���������ֱ��ͨ��
#endif

   	unsigned int l_crcProgram;
	unsigned int l_crcFlash;
	unsigned int l_paInputData;
	l_crcProgram = getProgramCRC()^readSerialID(1);  	//CRCA = CRC ^ SID2
	l_crcFlash = readFlashEncryptData(1);				//F_CRCB
	l_paInputData = (*((unsigned int *)0x40010808)); 	//PA
	return ((l_crcProgram^l_crcFlash)*(l_paInputData))%99999999;	//���������������ֵ���Ե�ǰPA�ڵ�����״̬������99999999ȡ�࣬�Ա�õ�һ�������ֵ��
}

/*
* ��оƬ���кŽ��м�飬ÿ��оƬ�����к���Ψһ��
* ������к���ȷ�򷵻�0�����򷵻�ֵ��ȷ����Ϊ0�Ŀ����Էǳ�С��
*/
unsigned int serialIDCheck(void)
{
#ifndef ENCRYPT_CHIP
	return 0; 		//���û�ж�����ܣ���ֱ�ӷ���0���������ֱ��ͨ��
#endif

	unsigned int l_sidFlash1 = readFlashEncryptData(2); 			//F_SID1B
	unsigned int l_sidFlash2 = readFlashEncryptData(3);				//F_SID2B
	unsigned int l_sidFlash3 = readFlashEncryptData(4);	  			//F_SID3B
	unsigned int l_sidRead1 = readSerialID(0)^readSerialID(1);		//SID1A = SID1 ^ SID2
	unsigned int l_sidRead2 = readSerialID(1)^readSerialID(2);		//SID2A = SID2 ^ SID3
	unsigned int l_sidRead3 = readSerialID(2)^readSerialID(0);		//SID1A = SID3 ^ SID1
	unsigned int l_pbInputData = (*((unsigned int *)0x40010C08));	//PB
	return (((l_sidFlash1^l_sidRead1) + (l_sidFlash2^l_sidRead2) + (l_sidFlash3^l_sidRead3))*l_pbInputData)%99999999; 	//��ȫ���ʱ�ŷ���0
}





