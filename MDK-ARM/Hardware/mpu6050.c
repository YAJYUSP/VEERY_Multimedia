#include "mpu6050.h" 
#include "mutex.h"
#include <main.h>		

imu_t imu0;


 //��i2c���ߵ�ʹ��Ȩ�л���mpu6050
uint8_t bsp_i2c_switch_to_mpu6050(void)
{
	//��ͼ����
	return(mutex_tryLock(&i2c_mutex, MPU6050));
}



//��ʼ��MPU6050
//����ֵ:0,�ɹ�
//����,�������
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
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
		HAL_Delay(5);
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//����MPU6050 
		HAL_Delay(5);
		MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
		HAL_Delay(5);
		MPU_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
		HAL_Delay(5);
		MPU_Set_Rate(50);						//���ò�����50Hz
		HAL_Delay(5);
		MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//�ر������ж�
		HAL_Delay(5);
		MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
		HAL_Delay(5);
		MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
		HAL_Delay(5);
		MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
		HAL_Delay(5);
		res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
		HAL_Delay(5);
		if(res==MPU_ADDR)//����ID��ȷ
		{
				MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
				HAL_Delay(5);
				MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
				HAL_Delay(5);
				MPU_Set_Rate(50);						//���ò�����Ϊ50Hz
		}else 
				return 1;
		return 0;
}



//����MPU6050����
void bsp_mpu6050_get_data(void)
{
		MPU_Get_Gyroscope(&imu0.gyro_yaw[xx],&imu0.gyro_yaw[yy],&imu0.gyro_yaw[zz]);
		MPU_Get_Accelerometer(&imu0.accel_yaw[xx],&imu0.accel_yaw[yy],&imu0.accel_yaw[zz]);
		imu0.temp_degree = MPU_Get_Temperature();
}



//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
static uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
		return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}



//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
static uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
		return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}


//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
static uint8_t MPU_Set_LPF(uint16_t lpf)
{
		uint8_t data=0;
		if(lpf>=188)data=1;
		else if(lpf>=98)data=2;
		else if(lpf>=42)data=3;
		else if(lpf>=20)data=4;
		else if(lpf>=10)data=5;
		else data=6; 
		return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}



//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
static uint8_t MPU_Set_Rate(uint16_t rate)
{
		uint8_t data;
		if(rate>1000)rate=1000;
		if(rate<4)rate=4;
		data=1000/rate-1;
		data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
		return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}



//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
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


//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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





//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
static uint8_t MPU_Read_Len(uint8_t reg,uint8_t len,uint8_t *buf)
{ 
		extern I2C_HandleTypeDef hi2c1;
		//�����л����߿���Ȩ
		bsp_i2c_switch_to_mpu6050();
		
		//��ѯ���߿���Ȩ
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 1;
		HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 0xfff);
		//�������߿���Ȩ
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return 0;
}
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
static uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
		extern I2C_HandleTypeDef hi2c1;
		unsigned char W_Data=0;
		W_Data = data;

		//�����л����߿���Ȩ
		bsp_i2c_switch_to_mpu6050();
		
		//��ѯ���߿���Ȩ
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 1;
		HAL_I2C_Mem_Write(&hi2c1, MPU_WRITE, reg, I2C_MEMADD_SIZE_8BIT, &W_Data, 1, 0xfff);
		//�������߿���Ȩ
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
static uint8_t MPU_Read_Byte(uint8_t reg)
{
		extern I2C_HandleTypeDef hi2c1;
		unsigned char R_Data=0;
		//�����л����߿���Ȩ
		bsp_i2c_switch_to_mpu6050();
		
		//��ѯ���߿���Ȩ
		if(!mutex_getDeviceAcc(&i2c_mutex, MPU6050))
			return 0xFF;
		HAL_I2C_Mem_Read(&hi2c1, MPU_READ, reg, I2C_MEMADD_SIZE_8BIT, &R_Data, 1, 0xfff);
		//�������߿���Ȩ
		mutex_tryUnlock(&i2c_mutex, MPU6050);
		return R_Data;	
}
