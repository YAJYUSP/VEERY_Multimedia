#include "stm32f4xx_hal.h"
#include "main.h"
#include "tim.h"
#include "encoder.h"


encoder_t encoder;


//初始化编码器
void bsp_encoder_init(void)
{
	 HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
}



//更新编码器计数值
void bsp_encoder_get(void)
{
		encoder.raw_cnt = htim2.Instance -> CNT;
		bsp_encoder_clear_cnt();
	
		//判断为编码器反转
		if(encoder.raw_cnt >= 16384)
		{
				encoder.dir = -1;
				encoder.speed = 32767 - encoder.raw_cnt;
				encoder.integral -= encoder.speed;
		}
		//判断为编码器正转
		else
		{
				encoder.dir = 1;
				encoder.speed = encoder.raw_cnt;
				encoder.integral += encoder.speed;
		}
}



//清空编码器计数值
void bsp_encoder_clear_cnt(void)
{
		htim2.Instance -> CNT = 0;
}
