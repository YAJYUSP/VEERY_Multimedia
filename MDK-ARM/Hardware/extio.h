#ifndef _EXTIO_H
#define _EXTIO_H

#include <usart.h>
#include <stdint.h>



//配置 拓展接口用途是uart2还是模拟iic
#define PORT_USAGE   1 /*  0:I2C  1:UART2  */








#if PORT_USAGE == 0

	#define SW_I2C_SCL_Pin  GPIO_PIN_3
	#define SW_I2C_SCL_Port GPIOA
	#define SW_I2C_SDA_Pin  GPIO_PIN_2
	#define SW_I2C_SDA_Port GPIOA

#endif

void bsp_extio_init(void);




#endif
