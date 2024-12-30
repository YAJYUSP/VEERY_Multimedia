#include <arm_math.h>
#include <main.h>
#include <spi.h>
#include "lcd.h"
#include "tim.h"
#include "lcdfont.h"


extern uint8_t LCD_GRAM[32768];//LCD graphic RAM defined external


/******************************************************************************
  * @brief  send GRAM to LCD(refresh whole screen)
	* @param  None
  * @retval None
******************************************************************************/
void LCD_RefreshGRAM(void)
{
	  extern DMA_HandleTypeDef hdma_spi3_tx;
	  
		LCD_Address_Set(0,0,127,127);
		while(HAL_DMA_GetState(&hdma_spi3_tx) != HAL_DMA_STATE_READY);
		HAL_SPI_Transmit_DMA(&hspi3,(uint8_t*)LCD_GRAM,32768);
}


/******************************************************************************
  * @brief  draw one point to lcd GRAM
	* @param  x(y): the x(y)-coordinate of the pixel , in a range of [0,127]
  * @retval None
******************************************************************************/
void LCD_GRAM_DrawPoint(u8 x,u8 y,u16 color)
{
		LCD_GRAM[256*(y-1)+(2*x)] = (color&0xFF00) >> 8;
	  LCD_GRAM[256*(y-1)+(2*x-1)] = (color&0xFF);
}


/******************************************************************************
  * @brief  read one point from lcd GRAM
	* @param  x(y): the x(y)-coordinate of the pixel , in a range of [0,127]
  * @retval color of the point
******************************************************************************/
uint16_t LCD_GRAM_Read(u8 x,u8 y)
{
		return (LCD_GRAM[256*(y-1)+(2*x)] << 8)+LCD_GRAM[256*(y-1)+(2*x-1)];
}


/******************************************************************************
  * @brief  Fill GRAM with pixels in selected area
	* @param  x(y)sta: the x(y)-coordinate of start pixel , in a range of [0,127]
  * @retval None
******************************************************************************/
void LCD_Fill_GRAM(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
		u16 i,j; 
		for(i=ysta;i<=yend+1;i++)
		{													   	 	
				for(j=xsta;j<=xend;j++)
				{
						LCD_GRAM_DrawPoint(j,i,color);
				}
		}
}


/******************************************************************************
  * @brief  Draw a line in the GRAM
	* @param  x(y): the x(y)-coordinate of start/end pixel , in a range of [0,127]
  * @retval None
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
		uint16_t t;
		int xerr = 0, yerr = 0, delta_x, delta_y, distance;
		int incx, incy, uRow, uCol;
		delta_x = x2 - x1; //计算坐标增量
		delta_y = y2 - y1;
		uRow = x1;
		uCol = y1;
		if (delta_x > 0)
				incx = 1; //设置单步方向
		else if (delta_x == 0)
				incx = 0; //垂直线
		else
		{
				incx = -1;
				delta_x = -delta_x;
		}
		if (delta_y > 0)
				incy = 1;
		else if (delta_y == 0)
				incy = 0; //水平线
		else
		{
				incy = -1;
				delta_y = -delta_y;
		}
		if (delta_x > delta_y)
				distance = delta_x; //选取基本增量坐标轴
		else
				distance = delta_y;
		for (t = 0; t <= distance + 1; t++) //画线输出
		{
				LCD_GRAM_DrawPoint(uRow, uCol, color); //画点
				xerr += delta_x;
				yerr += delta_y;
				if (xerr > distance)
				{
						xerr -= distance;
						uRow += incx;
				}
				if (yerr > distance)
				{
						yerr -= distance;
						uCol += incy;
				}
		}
}



/******************************************************************************
  * @brief  Draw a rectangle in the GRAM
	* @param  xn(yn): the xn(yn)-coordinate of start/end pixel , in a range of [0,127]
  * @retval None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
		LCD_DrawLine(x1,y1,x2,y1,color);
		LCD_DrawLine(x1,y1,x1,y2,color);
		LCD_DrawLine(x1,y2,x2,y2,color);
		LCD_DrawLine(x2,y1,x2,y2,color);
}



//显示进度条（左上角x，左上角y，进度条长度，进度条宽度，进度条延伸轴向，进度条延伸方向，进度条目前显示比例,是否显示边框）
//轴向ext:1——x轴  0——y轴
//方向dir：0为负向，1为正向
//比例为0-100（%）
void LCD_ShowLoading(u8 x, u8 y, u8 len, u8 wide, u8 ext, u8 dir, u8 ratio, u16 comc, u16 frac, u16 bgc)
{
	 if(ratio>100)return;
	 u8 threshold_H = len*ratio/100;
	 
	 if(ext)     //朝x轴延伸
	 {
				LCD_DrawLine(x,y,x+len,y,frac);
				LCD_DrawLine(x,y+wide,x+len+1,y+wide,frac);
				LCD_DrawLine(x,y,x,y+wide,frac);
				LCD_DrawLine(x+len+1,y,x+len+1,y+wide,frac);			 //画出边框
				
				
			if(dir)  //x轴正向延伸
			{
				 for(u8 i=y+1; i<y+wide; i++)
				 {
						 LCD_DrawLine(x,i,x+threshold_H,i,comc);       //填充进度条
						 if(threshold_H < len-1)
						 LCD_DrawLine(x+threshold_H+1,i,x+len-1,i,bgc);
				 }
			}
			else     //x轴负向延伸
			{
				  for(u8 i=y+1; i<y+wide; i++)
					{
						 LCD_DrawLine(x+len,i,x+len-threshold_H,i,comc); //填充进度条
						 if(threshold_H < len-1)
						 LCD_DrawLine(x+len-threshold_H-1,i,x+1,i,bgc);
					}	 
			}			
		}
		 
		 
	 else      //朝y轴延伸
		{
		     LCD_DrawLine(x,y,x,y+len,frac);
         LCD_DrawLine(x+wide,y,x+wide,y+len+1,frac);
			   LCD_DrawLine(x,y,x+wide+1,y,frac);
			   LCD_DrawLine(x,y+len,x+wide,y+len,frac);//画出边框
			 
			if(dir)  //y轴正向延伸
			 {
				 for(u8 i=x+1; i<x+wide; i++)
					 {
						 LCD_DrawLine(i,y,i,y+threshold_H,comc);         //填充进度条
						 if(threshold_H < len-1)
							 LCD_DrawLine(i,y+threshold_H+1,i,y+len-1,bgc);
					 }	 
			 }
			else     //y轴负向延伸
			 {
				  for(u8 i=x+1; i<x+wide; i++)
					 {
						 LCD_DrawLine(i,y+len,i,y+len-threshold_H,comc);//填充进度条
						 if(threshold_H < len-1)
							 LCD_DrawLine(i,y+len-threshold_H-1,i,y+1,bgc);
					 }	 
			 }			
		 }
 }

 

void LCD_Draw_Round(u8 x, u8 y, u8 r, u16 color)
{
		uint8_t vx,vy;
		float ang;
		for(ang=0; ang<2*PI; ang+=0.1)
		{
				vx = x+r*arm_cos_f32(ang);
				vy = y+r*arm_sin_f32(ang);
				LCD_GRAM_DrawPoint(vx,vy,color);
		}
}



/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式88889=
      返回值：  无
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
		u8 temp,sizex,t;
		u16 i,TypefaceNum;//一个字符所占字节大小
		u16 x0=x;
		sizex=sizey/2;
		TypefaceNum=sizex/8*sizey;
		num=num-' ';    //得到偏移后的值
		LCD_Fill_GRAM(x,y,x+sizey-1,y+sizey-1,bc);  //设置光标位置 
		for(i=0;i<TypefaceNum;i++)
		{ 
				if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
				else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
				else return;
				for(t=0;t<8;t++)
				{
						if(temp&(0x01<<t))LCD_GRAM_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizex)
						{
								x=x0;
								y++;
								break;
						}
				}
		}   	 	  
}



/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
		u8 t,temp;
		u8 enshow=0;
		u8 sizex=sizey/2;
		for(t=0;t<len;t++)
		{
				temp=(num/mypow(10,len-t-1))%10;
				if(enshow==0&&t<(len-1))
				{
						if(temp==0)
						{
								LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
								continue;
						}
						else 
								enshow=1; 
				}
				LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
		}
} 




/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowFloatNum(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
		u8 t,temp,sizex;
		u16 num1;
		sizex=sizey/2;
		num1=num*100;
		for(t=0;t<len;t++)
		{
				temp=(num1/mypow(10,len-t-1))%10;
				if(t==(len-2))
				{
						LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
						t++;
						len+=1;
				}
				LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
		}
}


/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组    
      返回值：  无
******************************************************************************/
extern DMA_HandleTypeDef hdma_spi3_tx;

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])//先发送像素地址，再挨个发送数组内容
{																																	 
		LCD_Address_Set(x,y,x+length-1,y+width-1);
		
		while(HAL_DMA_GetState(&hdma_spi3_tx) != HAL_DMA_STATE_READY);
		HAL_SPI_Transmit_DMA(&hspi3,(uint8_t*)pic,32768);
}



/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{         
		while(*p!='\0')
		{       
				LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
				x+=sizey/2;
				p++;
		}  
}



/******************************************************************************
  * @brief  Display a 24x24 custom char in the GRAM
	* @param  x(y): the x(y)-coordinate of the string , in a range of [0,127]
						*s:		address of the string to display
						fc:		font color
						sizey:font height
  * @retval None
******************************************************************************/
void LCD_ShowCustomChar(u16 x,u16 y,u8 *s,u16 fc,u16 bc)
{
		u8 i,j;
		u16 k;
		u16 HZnum;//汉字数目
		u16 TypefaceNum;//一个字符所占字节大小
		u16 x0=x;
		TypefaceNum=72;//此算法只适用于字宽等于字高，且字高是8的倍数的字，
															//也建议用户使用这样大小的字,否则显示容易出问题！
		HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字数目
		for(k=0;k<HZnum;k++) 
		{
				if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
				{ 	
						LCD_Fill_GRAM(x,y,x+24-1,y+24-1,bc);
						for(i=0;i<TypefaceNum;i++)
						{
								for(j=0;j<8;j++)
								{	
										if(tfont24[k].Msk[i]&(0x01<<j))	LCD_GRAM_DrawPoint(x,y,fc);//画一个点
										x++;
										if((x-x0)==24)
										{
												x=x0;
												y++;
												break;
										}
								}
						}
				}				  	
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
		}
} 


/******************************************************************************
  * @brief  Display a chinese string in the GRAM
	* @param  x(y): the x(y)-coordinate of the string , in a range of [0,127]
						*s:		address of the string to display
						fc:		font color
						sizey:font height
  * @retval None
******************************************************************************/

//void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u8 sizey)
//{
//	while(*s!=0)
//	{
//		if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,sizey);
//		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,sizey);
//		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,sizey);
//		else return;
//		s+=2;
//		x+=sizey;
//	}
//}



/******************************************************************************
      函数说明：显示单个32x32汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;//此算法只适用于字宽等于字高，且字高是8的倍数的字，
	                          //也建议用户使用这样大小的字,否则显示容易出问题！
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
      LCD_Fill_GRAM(x,y,x+sizey-1,y+sizey-1,bc);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_GRAM_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}


int my_abs(int ipt)
{
		if(ipt>=0)return ipt;
	  else return -ipt;
}

u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}



/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	  		
	  extern DMA_HandleTypeDef hdma_spi3_tx;
	  while(HAL_DMA_GetState(&hdma_spi3_tx) != HAL_DMA_STATE_READY);
	  HAL_SPI_Transmit_DMA(&hspi3,(uint8_t*)&dat,1);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
		LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
  LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	x1+=X_OFFSET;
	x2+=X_OFFSET;
	y1+=Y_OFFSET;
	y2+=Y_OFFSET;
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+3);
		LCD_WR_DATA(y2+3);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+3);
		LCD_WR_DATA(x2+3);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
}




void LCD_Init(void)
{	
	LCD_CS_Clr();
	LCD_RES_Clr();//复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);

	LCD_WR_REG(0x11); //Sleep out
	HAL_Delay(120);
	//------------------------------------ST7735S Frame rate-------------------------------------------------//
	LCD_WR_REG(0xB1); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB2); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB3); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	//------------------------------------End ST7735S Frame rate-------------------------------------------//
	LCD_WR_REG(0xB4); //Dot inversion
	LCD_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence-----------------------------------------//
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0xA2);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence---------------------------------------//
	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA8(0x0a);
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0XE0);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	LCD_WR_REG(0XE1);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0x3A); //65k mode
	LCD_WR_DATA8(0x05);
	//LCD_WR_REG(0x21); //控制反转颜色
	LCD_WR_REG(0x29); //Display on 
	//初始化LCD背光
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
}



//设置背光亮度，0~100
void bsp_lcd_bl_pwm_set(uint8_t compare)
{
		if(compare >= 100)
				return;
		__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,compare);
}
