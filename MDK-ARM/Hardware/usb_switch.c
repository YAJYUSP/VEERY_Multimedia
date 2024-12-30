#include "stm32f4xx_hal.h"
#include "main.h"
#include "usb_switch.h"


//切换板载typec口的功能
void bsp_typec_usb_func_switch(usb_func_e func)
{
		switch(func)
		{
				case USB_2_UART:
				//USB转串口
				HAL_GPIO_WritePin(USB_SW_GPIO_Port, USB_SW_Pin, GPIO_PIN_RESET);
						break;
				
				case USB:
				//USBHID
				HAL_GPIO_WritePin(USB_SW_GPIO_Port, USB_SW_Pin, GPIO_PIN_SET);
						break;
		}
}

