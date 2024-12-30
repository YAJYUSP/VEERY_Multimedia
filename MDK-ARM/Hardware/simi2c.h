#ifndef   _SIMI2C_H_
#define  _SIMI2C_H_

#include "main.h"
#include "extio.h"


#define IIC_WR	0		/* 写控制bit */
#define IIC_RD	  1		/* 读控制bit */


//当将外部拓展接口作调试串口之用，跳过一切软I2C读写操作
#if PORT_USAGE == 1

#define IIC_SCL_1()  	0
#define IIC_SCL_0()   0

#define IIC_SDA_1()  0
#define IIC_SDA_0()  0

#define IIC_SDA_READ()   0
#define IIC_Delay()     0

#elif PORT_USAGE == 0

#define IIC_SCL_1()  	HAL_GPIO_WritePin(SW_I2C_SCL_Port, SW_I2C_SCL_Pin, GPIO_PIN_SET)		    /* SCL = 1 */
#define IIC_SCL_0()    HAL_GPIO_WritePin(SW_I2C_SCL_Port, SW_I2C_SCL_Pin, GPIO_PIN_RESET)		/* SCL = 0 */

#define IIC_SDA_1()  HAL_GPIO_WritePin(SW_I2C_SDA_Port, SW_I2C_SDA_Pin, GPIO_PIN_SET)	  	/* SDA = 1 */
#define IIC_SDA_0()  HAL_GPIO_WritePin(SW_I2C_SDA_Port, SW_I2C_SDA_Pin, GPIO_PIN_RESET)  /* SDA = 0 */

#define IIC_SDA_READ()   HAL_GPIO_ReadPin(SW_I2C_SDA_Port, SW_I2C_SDA_Pin)								/* 读SDA口线状态 */
#define IIC_Delay()      bsp_tim9_delay_us(50);

#endif



//引脚定义
/*
 * SCL——PB6
 * SDA——PB7
 */
#define IIC1_SDA_IN()	//{GPIOA->MODER &= ~(3<<(2*2)); GPIOA->MODER |= 0<<2*2;} //PB7输入模式
#define IIC1_SDA_OUT()	//{GPIOA->MODER &= ~(3<<(2*2)); GPIOA->MODER |= 1<<2*2;} //PB7输出模式


#include "stdint.h"


void IIC_NAck(void);
void IIC_Ack(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Stop(void);
void IIC_Start(void);

uint8_t IIC_Read_Byte(uint8_t ack);
void IIC_Send_Byte(uint8_t _ucByte);

uint8_t IIC_CheckDevice(uint8_t _Address);
uint8_t bsp_simi2c_write_len(uint8_t dev_addr, uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t bsp_simi2c_read_len(uint8_t dev_addr, uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t bsp_simi2c_write_byte(uint8_t dev_addr, uint8_t reg,uint8_t data);
uint8_t bsp_simi2c_read_byte(uint8_t dev_addr, uint8_t reg);


#endif
