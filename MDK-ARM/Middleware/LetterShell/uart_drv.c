#include <stdlib.h>
#include <stdio.h>
#include <main.h>
#include "uart_drv.h"
#include "usart.h"
#include "extio.h"


//����ⲿ�˿���UART2֮�ã���ʹ��UART2��Ϊ���Զ˿ڣ���Ȼʹ��TYPEC�ϵ�UART1
#if PORT_USAGE == 1

#define SHELL_COM    &huart2

//�ض���printf������2
typedef struct __FILE FILE;
int fputc(int ch,FILE *f)
{
		HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,0xFFFF);
		return ch;
}

#elif PORT_USAGE == 0

#define SHELL_COM    &huart1
//�ض���printf������1
typedef struct __FILE FILE;
int fputc(int ch,FILE *f)
{
		HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);
		return ch;
}

#endif

 

SHELL_TypeDef shell0;
uint8_t uart_rx_data = 0x00;


void uart_send_string(const char chr)
{
	HAL_UART_Transmit(SHELL_COM, (const uint8_t *)&chr, 1, 0xffff);
}

// ��ʼ��shell�Լ��������
void shell_init(void)
{
	HAL_UART_Receive_IT(SHELL_COM, &uart_rx_data, 1);
	shell0.write = uart_send_string;
	
	shellInit(&shell0);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == SHELL_COM)
	{
		shellInput(&shell0, uart_rx_data);
		HAL_UART_Receive_IT(SHELL_COM, &uart_rx_data, 1);
	}
}
