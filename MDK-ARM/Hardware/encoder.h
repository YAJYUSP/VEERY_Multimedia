#ifndef _ENCODER_H
#define _ENCODER_H

typedef struct
{
		//ԭʼ�Ĵ���ֵ
		uint16_t raw_cnt;
		//ת���ٶȾ���ֵ
		uint16_t speed;
		//�ٶȻ���ֵֵ
		uint32_t integral;
		//ת������ֵ,-1Ϊ����1Ϊ��
		int8_t dir;
}encoder_t;

extern encoder_t encoder;

void bsp_encoder_init(void);
void bsp_encoder_clear_cnt(void);
void bsp_encoder_get(void);

#endif
