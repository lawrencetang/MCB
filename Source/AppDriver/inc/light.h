/*
µÆ¿ØÖÆ
20140518
*/

#ifndef __MY_LIGHT_H
#define __MY_LIGHT_H

#include "my_frame.h"
#include "stm32f10x_gpio.h"


#define P_DLAMP_C0			GPIO_Pin_9	  		/* PG9 */
#define P_DLAMP_C1			GPIO_Pin_10 	  	/* PG10 */	
	

#define OUT1_DLAMP_C0 		GPIO_SetBits(GPIOG,P_DLAMP_C0) 
#define OUT1_DLAMP_C1 		GPIO_SetBits(GPIOG,P_DLAMP_C1) 


#define OUT0_DLAMP_C0 		GPIO_ResetBits(GPIOG,P_DLAMP_C0) 
#define OUT0_DLAMP_C1 		GPIO_ResetBits(GPIOG,P_DLAMP_C1) 


/* ¿ªë®µÆ */
void L_DLampOn(void);

/* ¹Øë®µÆ */
void L_DLampOff(void);

/* ¿ªÎÙµÆ */
void L_WLampOn(void);

/* ¹ØÎÙµÆ */
void L_WLampOff(void);


void L_Init(void);


u8 LAMP_Ctrl(Original_Frame* o_frame);

#endif





