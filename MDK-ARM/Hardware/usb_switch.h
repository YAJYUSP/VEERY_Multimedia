#ifndef _USB_SWITCH_H
#define _USB_SWITCH_H

//TYPEC USB功能枚举体
typedef enum
{
		USB,
		USB_2_UART
}usb_func_e;

void bsp_typec_usb_func_switch(usb_func_e func);


#endif
