#include "mpu6050.h" 
#include "mutex.h"
#include <main.h>		

imu_t imu0;


 //将i2c总线的使用权切换到mpu6050
uint8_t bsp_i2c_switch_to_mpu6050(void)
{
	//试图上锁
	return(mutex_tryLock(&i2c_mutex, MPU6050));
}



//初始化MPU6050
//返回值:0,成功
//其他,错误代码
uint8_t bsp_mpu6050_init(void)
{
		for(uint8_t i=0; i<3; i++)
		{
				imu0.accel_yaw[i] = 0;
				imu0.gyro_yaw[i] = 0;
				imu0.euler_angle[i] = 0;
		}
		imu0.temp_degree = 0;
		
		uint8_t res;
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
		HAL_Delay(5);
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
		HAL_Delay(5);
		MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
		HAL_Delay(5);
		MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
		HAL_Delay(5);
		MPU_Set_Rate(50);						//设置采样率50Hz
		HAL_Delay(5);
		MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
		HAL_Delay(5);
		MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
		HAL_Delay(5);
		MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
		HAL_Delay(5);
		MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
		HAL_Delay(5);
		res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
		HAL_Delay(5);
		if(res==MPU_ADDR)//器件ID正确
		{
				MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
				HAL_Delay(5);
				MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
				HAL_Delay(5);
				MPU_Set_Rate(50);						//设置采样率为50Hz
		}else 
				return 1;
		return 0;
}



//更新MPU6050数据
void bsp_mpu6050_get_data(void)
{
		MPU_Get_Gyroscope(&imu0.gyro_yaw[xx],&imu0.gyro_yaw[yy],&imu0.gyro_yaw[zz]);
		MPU_Get_Accelerometer(&imu0.accel_yaw[xx],&imu0.accel_yaw[yy],&imu0.accel_yaw[zz]);
		imu0.temp_degree = MPU_Get_Temperature();
}



//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
static uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
		return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}



//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
static uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
		return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}


//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
static uint8_t MPU_Set_LPF(uint16_t lpf)
{
		uint8_t data=0;
		if(lpf>=188)data=1;
		else if(lpf>=98)data=2;
		else if(lpf>=42)data=3;
		else if(lpf>=20)data=4;
		else if(lpf>=10)data=5;
		else data=6; 
		return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}



//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
static uint8_t MPU_Set_Rate(uint16_t rate)
{
		uint8_t data;
		if(rate>1000)rate=1000;
		if(rate<4)rate=4;
		data=1000/rate-1;
		data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
		return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}



//得到温度值
//返回值:温度值(扩大了100倍)
static float MPU_Get_Temperature(void)
{
		unsigned char  buf[2]; 
		short raw;
		float temp;
		
		MPU_Read_Len(MPU_TEMP_OUTH_REG,2,buf); 
		raw=(buf[0]<<8)| buf[1];  
		temp=(36.53+((double)raw)/340)*100;  
		return temp/100.0f;
}


//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
static uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
		uint8_t buf[6],res;  
		res=MPU_Read_Len(MPU_GYRO_XOUTH_REG,6,buf);
		if(res==0)
		{
				*gx=((uint16_t)buf[0]<<8)|buf[1];  
				*gy=((uint16_t)buf[2]<<8)|buf[3];  
				*gz=((uint16_t)buf[4]<<8)|buf[5];
		} 	
		return res;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
static uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
		uint8_t buf[6],res;  
		res=MPU_Read_Len(MPU_ACCEL_XOUTH_REG,6,buf);
		if(res==0)
		{
				*ax=((uint16_t)buf[0]<<8)|buf[1];  
				*ay=((uint16_t)buf[2]<<8)|buf[3];  
				*az=((uint16_t)buf[4]<<8)|buf[5];
		} 	
				return res;
}





//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
static uint8_t MPU_Read_Len(uint8_t reg,uint8_t len,uint8_t *buf)
{ 
		extern I2C_HandleTypeDef hi2c1;
		//尝试切换总线控制权
		bsp_i2c_switch_to_mpu6050();
		
		//查询总线控制权
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 1;
		HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xfff);
		//放弃总线控制权
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return 0;
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
static uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
		extern I2C_HandleTypeDef hi2c1;
		unsigned char W_Data=0;
		W_Data = data;

		//尝试切换总线控制权
		bsp_i2c_switch_to_mpu6050();
		
		//查询总线控制权
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 1;
		HAL_I2C_Mem_Write(&hi2c1, MPU_WRITE, reg, I2C_MEMADD_SIZE_8BIT, &W_Data, 1, 0xfff);
		//放弃总线控制权
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
static uint8_t MPU_Read_Byte(uint8_t reg)
{
		extern I2C_HandleTypeDef hi2c1;
		unsigned char R_Data=0;
		//尝试切换总线控制权
		bsp_i2c_switch_to_mpu6050();
		
		//查询总线控制权
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 0xFF;
		HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, &R_Data, 1, 0xfff);
		//放弃总线控制权
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return R_Data;	
}
