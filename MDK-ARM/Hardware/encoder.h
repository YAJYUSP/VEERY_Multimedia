#ifndef _ENCODER_H
#define _ENCODER_H

typedef struct
{
		//原始寄存器值
		uint16_t raw_cnt;
		//转动速度绝对值
		uint16_t speed;
		//速度积分值值
		uint32_t integral;
		//转动方向值,-1为反，1为正
		int8_t dir;
}encoder_t;

extern encoder_t encoder;

void bsp_encoder_init(void);
void bsp_encoder_clear_cnt(void);
void bsp_encoder_get(void);

#endif
