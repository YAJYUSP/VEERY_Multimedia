#ifndef __LCD_H
#define __LCD_H		

#include <stdint.h>

#define USE_HORIZONTAL 1  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 128
#define X_OFFSET 0
#define Y_OFFSET -2
#else
#define LCD_W 128
#define LCD_H 128
#endif

//-----------------LCD�˿ڶ���---------------- 

#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)


typedef short u16;
typedef unsigned char u8;

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��

typedef short u16;
typedef unsigned char u8;
typedef long u32;

void LCD_RefreshGRAM(void);
void LCD_GRAM_DrawPoint(u8 x,u8 y,u16 color);
uint16_t LCD_GRAM_Read(u8 x,u8 y);
void LCD_Fill_GRAM(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//ָ�����������ɫ

int my_abs(int ipt);
u32 mypow(u8 m,u8 n);//����

void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//��ָ��λ�û�һ����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//��ָ��λ�û�һ������
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//��ָ��λ�û�һ��Բ

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u8 sizey);
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����16x16����
void LCD_ShowCustomChar(u16 x,u16 y,u8 *s,u16 fc,u16 bc);
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����32x32����

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾһ���ַ�
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ�ַ���
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��������
void LCD_ShowFloatNum(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��λС������

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//��ʾͼƬ
void LCD_Draw_Round(u8 x, u8 y, u8 r, u16 color);
void LCD_ShowLoading(u8 x, u8 y, u8 len, u8 wide, u8 ext, u8 dir, u8 ratio, u16 comc, u16 frac, u16 bgc);


void bsp_lcd_bl_pwm_set(uint8_t compare);


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


//#define PI 3.14
#endif




