

#ifndef _ENCRYPT_CHIP_H
#define _ENCRYPT_CHIP_H

/* �����Ƿ��оƬ���м��� */
#define ENCRYPT_CHIP 		1

/* оƬ�ڲ��Ĵ�����ַ */
#define ADDR_DEVECEID 		0xE0042000 		//�豸ID��ַ��32λоƬ�ͺ�
#define ADDR_SERIALID 		0x1FFFF7E8 		//оƬ���кŵ�ַ��96λΨһID
#define ADDR_CRC 			0x40023000 		//CRC-32���ݶ�д��ַ��32λ����

/* ��ż������ݵĵ�ַ����Ҫ160λ��20�ֽڵ������ռ� */
#define ADDR_FLASH_ENCRYPT 	0x08010000 		//�豸ID��ַ��32λоƬ�ͺţ������64Kλ�ú�


#define ADDR_CRC_START 		0x08000000 		//����CRCУ����ʼ��ַ
#define ADDR_CRC_END 		0x08010000 		//����CRCУ�������ַ���������õ�ַ��У��64K����


#define DIVICE_ID 			0x59534859 		//�����豸��
#define ENCRYPT_KEY 		0x3579ABCD 		//�Զ�����Կ


/*
*  �������������
*  һ�ϵ���ֹ���򱻶���
*/
void flashReadProtect(void);


/*
* ���豸��DID
* ����32λ�豸ID
*/
unsigned int readDeviceID(void);

/*
* �����к�SID�������ζ�
* id���κ� 0 SID1��1 SID2��2 SID3
*/
unsigned int readSerialID(unsigned char id);


/*
* ��������CRC���
* ����������ε�ַ��ADDR_CRC_START��ADDR_CRC_END֮���CRCУ����
* ��оƬ���к�Ҳ���뵽CRC�����У�ʹÿ��оƬ��CRC����������һ��
* ����ֵ��32λCRCУ����
*/
unsigned int getProgramCRC(void);


/*
* ��FLASH
* һ���Խ�20�ֽ�ȫ�������������û���ȥ��ɢ���õ����Ǽ��ܺ������
*/
void readFlashData(unsigned char *l_a);


/*
* дFLASH
* һ���Խ�20�ֽ�ȫ��д�룬���������Ǽ������ݣ�������ɢ���û���浽FLASH��
*/
void writeFlashData(unsigned char *l_a);


/*
* ��FLASH�д洢�ļ��ܺ������
* �豸ID��4�ֽڹ�32λ��flag = 0��
* ����CRC��4�ֽڹ�32λ��flag = 1��
* ��źţ�12�ֽڹ�96λ��flag = 2, 3, 4�������η��ء�
*/
unsigned int readFlashEncryptData(unsigned char flag);


/*
* �����ܺ������д�뵽FLASH��
* �豸ID��4�ֽڹ�32λ��flag = 0��
* ����CRC��4�ֽڹ�32λ��flag = 1��
* ��źţ�12�ֽڹ�96λ��flag = 2, 3, 4��������д�롣
*/
unsigned char writeFlashEncryptData(unsigned int data, unsigned char flag);


/*
* ��оƬ���豸�Ž��м�飬�ͺ���ͬ��оƬ�豸����ͬ
* оƬ���豸����ȷ�򷵻�0�����򷵻�ֵΪһ���ϴ������
*/
unsigned int deviceIDCheck(void);


/*
* �Գ���ε�CRC����У��
* ���CRCУ����ȷ�򷵻�0�����򷵻�ֵ��ȷ����Ϊ0�Ŀ����Էǳ�С��
*/
unsigned int crcCheck(void);


/*
* ��оƬ���кŽ��м�飬ÿ��оƬ�����к���Ψһ��
* ������к���ȷ�򷵻�0�����򷵻�ֵ��ȷ����Ϊ0�Ŀ����Էǳ�С��
*/
unsigned int serialIDCheck(void);


#endif


