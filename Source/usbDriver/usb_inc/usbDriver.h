/*
usbDriver.h
*/

#ifndef _USB_DRIVER_H
#define _USB_DRIVER_H

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"



extern uint8_t g_USBRxBuff[VIRTUAL_COM_PORT_DATA_SIZE];
extern __IO uint32_t g_USBRxCNT;
extern uint32_t count_in;

#endif


