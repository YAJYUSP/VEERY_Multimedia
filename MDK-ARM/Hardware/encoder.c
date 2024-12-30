#include "stm32f4xx_hal.h"
#include "main.h"
#include "tim.h"
#include "encoder.h"


encoder_t encoder;


//��ʼ��������
void bsp_encoder_init(void)
{
	 HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
}



//���±���������ֵ
void bsp_encoder_get(void)
{
		encoder.raw_cnt = htim2.Instance -> CNT;
		bsp_encoder_clear_cnt();
	
		//�ж�Ϊ��������ת
		if(encoder.raw_cnt >= 16384)
		{
				encoder.dir = -1;
				encoder.speed = 32767 - encoder.raw_cnt;
				encoder.integral -= encoder.speed;
		}
		//�ж�Ϊ��������ת
		else
		{
				encoder.dir = 1;
				encoder.speed = encoder.raw_cnt;
				encoder.integral += encoder.speed;
		}
}



//��ձ���������ֵ
void bsp_encoder_clear_cnt(void)
{
		htim2.Instance -> CNT = 0;
}
