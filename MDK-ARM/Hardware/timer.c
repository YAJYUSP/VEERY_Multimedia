#include "stm32f4xx_hal.h"
#include "main.h"
#include "tim.h"
#include "timer.h"
#include "encoder.h"

//��ʼ��TIM11 5ms ISR
void bsp_tim11_5ms_isr_init(void)
{
		HAL_TIM_Base_Start_IT(&htim11);
}


//tim11��Ƶ������
uint8_t tim11_50ms_div_cnt = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //TIM11�ж����ڣ�5ms
{	
    if (htim == (&htim11))                           
    {   			
				if(tim11_50ms_div_cnt < 10)
						tim11_50ms_div_cnt++;
				else
				{
						tim11_50ms_div_cnt = 0;
						bsp_encoder_get();
				}
			
					
				
		}
		
		if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
}



//tim9 us��ʱ
void bsp_tim9_delay_us(uint16_t nus)
{
		__HAL_TIM_SetCounter(&htim9, 0);
		__HAL_TIM_ENABLE(&htim9);
	
		while(__HAL_TIM_GetCounter(&htim9) < nus);
		__HAL_TIM_DISABLE(&htim9);
}


