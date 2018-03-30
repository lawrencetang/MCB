#define  BSP_GLOBALS
#include <includes.h>

#include "system_stm32f10x.h"

#include "stm32f10xDriver.h"
#include "AppDriver.h"

#include "TaskInit.h"

#include "usb_lib.h"
#include "hw_config.h"


#define ADC1_DR_Address    ((u32)0x4001244C)
		   


ErrorStatus HSEStartUpStatus;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);//lt:HSE��

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//lt:ʹ��flashԤȡָ����

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);//lt:���ô�����ʱ������
  
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); //lt:ǰ��systeminit()������,����AHB��Ƶ
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); //lt:ǰ��systeminit()������

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2); //lt:ǰ��systeminit()������

    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
    /* PLLCLK = 8MHz * 7 = 56 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//lt:PLLʱ��ԴΪHSEʱ��,7��Ƶ//ltg

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);	

  }
}






/*
*********************************************************************************************************
* ������: BSP_Init
*
* ����   ��������ʼ��Ӳ���ϵ�����
*********************************************************************************************************
*/
void BSP_Init(void)
{
	 /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	 /* ��ϵͳ�У�ϵͳ��ʱ��Ϊ72MHz��PCLK1 = 36MHz��PCLK2 = 72MHz */
	SystemInit();//lt:����ϵͳ��ʱ��Ϊ72mhz���������ļ�û�е���systeminit����,define��STM32F10X_HD
	
	RCC_Configuration();//lt:����ϵͳʱ��Ϊ56MHz?


	 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_CRC, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1  | RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
						 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
	


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);//lt:ʹ����Χ�豸ʱ��
	
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);//lt:ѡ��USB��Ƶ1.5��usbƵ��48MHz

	powerON();

	USB_Interrupts_Config();
	USB_Init();

	ledINIT();
	fanInit();
	iicInit();
	uart1Init();
	absEncInit();
	smInit();
	dcmInit();	
	temAppInit();
//	Timer1Init();
	Timer2Init();
	Timer3Init();   
//	Timer4Init();


	NVIC_Configuration(); // ���ȼ����ã���Ҫ������232��422���³���ǰ	
 
    RCC_ClearFlag(); // ��RCC��־
}




