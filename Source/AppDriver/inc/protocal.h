/*
2015.05.15
*/

#ifndef _MY_PROTOCAL_H
#define _MY_PROTOCAL_H

/****************************************************/
/******************** ֡��ʶ���� ********************/
/****************************************************/
#define DOWN_FRAME 					0xC0
#define UP_FRAME 						0xB0


/****************************************************/
/******************** ���������� ********************/
/****************************************************/
/**************����ָ��**************/
#define COM_TEST 						0x00 /* ͨ�Ų��� */
#define FAN2_TEST						0x01 /* ����2���� */
#define EEPROM_READ_TEST		0x02 /* дEEPROM */
#define EEPROM_WRITE_TEST		0x03 /* ��EEPROM */
#define READ_AUTO_TEST			0x04 /* ��Ѱ�Զ����Կ���״̬ */


/**************��Ѱָ��**************/
#define BOARD_STATUS 				0x70 /* ��Ѱ����״̬ */
#define READ_HDW_VERSION		0x71 /* ��ѰӲ���汾 */
#define READ_SOFT_VERSION		0x72 /* ��Ѱ����汾 */
#define SET_PRODUCT_KIND		0x74 /* ���ò�Ʒ���� */
#define READ_SERIAL_ID  		0x75 /* ��ȡоƬ���к� */
#define SHUTDOWN_MAINBOARD  0x76 /* �ػ� */


/**************�������ָ��**************/
#define SM_CMD 							0x80 /* �������������� */
#define SM_CHECK 						0x80 /* ����������ϲ�Ѱ */
#define SM_START  					0x81 /* �������ת����ָ���Ƕ� */
#define SM_STOP  						0x82 /* ǿ��ֹͣ������� */
#define SM_READAPOS					0x83 /* ��ȡ���������ǰ�Ƕ� */
#define SM_READZERO  				0x84 /* ��ȡ��������߼���� */
#define SM_SETAERO  				0x85 /* ���ò�������߼���� */

/**************ֱ�����ָ��**************/
#define DCM_CMD 						0x90 /* ֱ������������ */
#define DCM_CHECK 					0x90 /* ֱ��������ϲ�Ѱ */
#define DCM_START  					0x91 /* ����ֱ����� */
#define DCM_STOP  					0x92 /* ֱֹͣ����� */


/**************���¿���ָ��**************/
#define TEM_CMD 						0xA0 /* ���²������ */
#define TEM_CHECK 					0xA0 /* ���¹��ϲ�Ѱ */
#define TEM_START  					0xA1 /* �������� */
#define TEM_STOP  					0xA2 /* ֹͣ���� */
#define TEM_READ  					0xA3 /* ����ǰ�¶� */




/****************************************************/
/******************* �������״̬ *******************/
/****************************************************/
/**************��λ��ָ��������**************/
#define STS_SUCCESS	 				0x00 /* �ɹ� */
#define STS_FAILED	 				0x01 /* ʧ�� */
#define STS_BUZY	 					0x02 /* ��æ */
#define STS_CMD_ERROR 			0x03 /* ָ����� */
#define STS_DEV_ERROR 			0x04 /* �豸��Ŵ��� */
#define STS_DATA_ERROR 			0x05 /* �������� */
#define STS_FAULT 					0x06 /* �豸���� */

#define NO_ERROR 						0x00 /* û�д��� */




/**************�������ָ��**************/
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


