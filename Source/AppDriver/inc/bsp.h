#ifndef  __BSP_H__
#define  __BSP_H__


#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif

#define TP_CS()  GPIO_ResetBits(GPIOG,GPIO_Pin_11)
#define TP_DCS() GPIO_SetBits(GPIOG,GPIO_Pin_11)
#define WaitTPReady() while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0)


#ifdef __cplusplus
#define     __I     volatile                  /*!< defines 'read only' permissions      */
#else
#define     __I     volatile const            /*!< defines 'read only' permissions      */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

#define   RS422_LED   0x0800
#define   RS232_LED   0x1000
#define   PSAM_LED    0x2000
#define   RUN_LED     0x4000

#define USART1_BPS  		115200
#define USART3_BPS  		115200 << 2
#define USART3_UPDATA_BPS  	115200


void BSP_Init(void);
//
void EXTI_Configuration(void);
//
void DMA_Configuration(void);

void ALL_LEDInit(void);

void RCC_Configuration(void);


#endif                            
