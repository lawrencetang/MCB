/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "usb_lib.h"
//#include "usb_desc.h"
//#include "usb_mem.h"
//#include "hw_config.h"
//#include "usb_istr.h"

#include "usbDriver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t g_USBRxBuff[VIRTUAL_COM_PORT_DATA_SIZE];
__IO uint32_t g_USBRxCNT = 0;
uint32_t count_in = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP3_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  g_USBRxCNT = GetEPRxCount(ENDP3);//lt:得到端点3接收到的数据的数量
  PMAToUserBufferCopy(g_USBRxBuff, ENDP3_RXADDR, g_USBRxCNT);//lt:从端点3的接收地址拷贝相应数量的数据到usermemory地址
  SetEPRxValid(ENDP3);
}//lt:端点3正确接收后调用

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  count_in = 0;//lt:端点1正确发送后调用
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

