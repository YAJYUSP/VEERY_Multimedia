#include "stm32f4xx_hal.h"
#include "main.h"
#include "usb_switch.h"


//�л�����typec�ڵĹ���
void bsp_typec_usb_func_switch(usb_func_e func)
{
		switch(func)
		{
				case USB_2_UART:
				//USBת����
				HAL_GPIO_WritePin(USB_SW_GPIO_Port, USB_SW_Pin, GPIO_PIN_RESET);
						break;
				
				case USB:
				//USBHID
				HAL_GPIO_WritePin(USB_SW_GPIO_Port, USB_SW_Pin, GPIO_PIN_SET);
						break;
		}
}

