#include "w25q64.h"
#include "mutex.h"
#include <stdlib.h>
#include <stdio.h>
#include <main.h>
#include <spi.h>



 //将spi总线的使用权切换到w25qxx
uint8_t bsp_spi_switch_to_w25q(void)
{
	//试图上锁
	if(mutex_tryLock(&spi_mutex, W25QXX))
		return 1;
	//切换总线速度
	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	HAL_SPI_Init(&hspi2);
}


 
 /**********************************************************************************
  * 函数功能: 模块初始化
  */
uint8_t bsp_w25q_init(void)
{ 	
		bsp_w25q_reset();	
	 /* 延时一小段时间 */
		HAL_Delay(10);
		/* 唤醒SPI Flash */
		bsp_w25q_wakeup();
		/* 获取SPI Flash芯片状态 */
		return bsp_w25q_get_status();
}



/**num:0~n*******/
void W25Q64_WriteImg(uint8_t num ,uint32_t *ImgBuf)
{
		printf("\r\n SPI-W25Qxx writing img \r\n\r\n");
		uint32_t ImgAddr = PIC_DATA_ADDR_BASE + num * HBLOCK_SIZE +1;
		/*-Step2- 擦除块  ************************************************Step2*/ 	
		if(bsp_w25q_erase(ImgAddr,HBLOCK_ERASE_CMD) == W25Qx_OK)
			printf(" SPIFlash erase block OK!\r\n");
		else
			printf("something wrong in erasing\r\n");
		/*-Step3- 写数据  ************************************************Step3*/	
		if(bsp_w25q_write((uint8_t *)ImgBuf,ImgAddr,HBLOCK_SIZE)== W25Qx_OK)
			printf(" SPIFlash writing image OK!\r\n");
		else
			printf("something wrong in writing\r\n");
}


/* Read out image data to LCD_GRAM ,num=0~n*/
void w25q_read_pic_to_GRAM(uint8_t num)
{
	  extern uint8_t LCD_GRAM[32768];
	  uint32_t ImgAddr = PIC_DATA_ADDR_BASE + num * HBLOCK_SIZE +1;
	
		printf("\r\n SPI-W25Q reading picture \r\n\r\n");
		if(bsp_w25q_read(LCD_GRAM,ImgAddr,HBLOCK_SIZE)== W25Qx_OK)
			printf(" SPI-W25Q read ok\r\n\r\n");
		else
			printf("something wrong in reading\r\n");
}


void bsp_w25q_test(void)
{
		uint8_t wData[0x100];   //写缓存数组
		uint8_t rData[0x100];   //读缓存数组
		uint8_t ID[4];          //设备ID缓存数组
		uint32_t i;

		printf("\r\n SPI-W25Qxx Self Check \r\n\r\n");
		/*-Step1- 验证设备ID  ************************************************Step1*/ 
		bsp_w25q_readID(ID);
		 //第一位厂商ID固定0xEF,第二位设备ID根据容量不同,具体为：
		 //W25Q16为0x14、32为0x15、40为0x12、64为0x16、80为0x13、128为0x17
		if((ID[0] != 0xEF) | (ID[1] != 0x16)) 
		{                                
				printf("something wrong in ChipID \r\n");
		}
		printf(" W25Qxx ID is : ");
		for(i=0;i<2;i++)
		{
				printf("0x%02X ",ID[i]);
		}
		printf("\r\n");
		/*-Step2- 擦除块  ************************************************Step2*/ 	
		if(bsp_w25q_erase(0,HBLOCK_ERASE_CMD) == W25Qx_OK)
				printf(" SPI Erase Block OK!\r\n");
		else
				printf("something wrong in Erasing\r\n");
		/*-Step3- 写数据  ************************************************Step3*/	
		for(i =0;i<0x100;i ++)
		{
				wData[i] = i;
				rData[i] = 0;
		}
		if(bsp_w25q_write(wData,0x00,0x100)== W25Qx_OK)
				printf(" SPI Write data OK!\r\n");
		else
				printf("something wrong in Writing\r\n");
			/*-Step4- 读数据  ************************************************Step4*/	
		if(bsp_w25q_read(rData,0x00,0x100)== W25Qx_OK)
				printf(" SPI Read data ok\r\n\r\n");
		else
				printf("something wrong in Reading\r\n");
		
		printf("SPI Read Data : \r\n");
		for(i =0;i<0x100;i++)
				printf("0x%02X  ",rData[i]);
		printf("\r\n\r\n");
		/*-Step5- 数据对比  ************************************************Step5*/		
		if(memcmp(wData,rData,0x100) == 0 ) 
				printf(" W25Q SPI Test OK\r\n");
		else
				printf(" W25Q SPI Test False\r\n");
	
}

void	bsp_w25q_reset(void)
{
		uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};
		
		W25Qx_Enable();
		/* Send the reset command */
		SPI_Transmit(&hspi2, cmd, 2, W25Qx_TIMEOUT_VALUE);	
		W25Qx_Disable();
}
 
 /**********************************************************************************
  * 函数功能: 获取设备状态
  */
uint8_t bsp_w25q_get_status(void)
{
		uint8_t cmd[] = {READ_STATUS_REG1_CMD};
		uint8_t status;
		
		W25Qx_Enable();
		/* Send the read status command */
		SPI_Transmit(&hspi2, cmd, 1, W25Qx_TIMEOUT_VALUE);	
		/* Reception of the data */
		SPI_Receive(&hspi2,&status, 1, W25Qx_TIMEOUT_VALUE);
		W25Qx_Disable();
		
		/* Check the value of the register */
		if((status & W25Q128FV_FSR_BUSY) != 0)
		{
				return W25Qx_BUSY;
		}
		else
		{
				return W25Qx_OK;
		}		
}
 
 /**********************************************************************************
  * 函数功能: 写使能
  */
uint8_t bsp_w25q_writeEnable(void)
{
		uint8_t cmd[] = {WRITE_ENABLE_CMD};
		uint32_t tickstart = HAL_GetTick();
	 
		/*Select the FLASH: Chip Select low */
		W25Qx_Enable();
		/* Send the read ID command */
		SPI_Transmit(&hspi2, cmd, 1, W25Qx_TIMEOUT_VALUE);	
		/*Deselect the FLASH: Chip Select high */
		W25Qx_Disable();
		
		/* Wait the end of Flash writing */
		while(bsp_w25q_get_status() == W25Qx_BUSY);
		{
				/* Check for the Timeout */
				if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
				{        
						return W25Qx_TIMEOUT;
				}
		}
		
		return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 获取设备ID
  */
void bsp_w25q_readID(uint8_t *ID)
{
		uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
		
		W25Qx_Enable();
		/* Send the read ID command */
		SPI_Transmit(&hspi2, cmd, 4, W25Qx_TIMEOUT_VALUE);	
		/* Reception of the data */
		SPI_Receive(&hspi2,ID, 2, W25Qx_TIMEOUT_VALUE);
		W25Qx_Disable();	
}
 
 /**********************************************************************************
  * 函数功能: 读数据
  * 输入参数: 缓存数组指针、读地址、字节数
  */
uint8_t bsp_w25q_read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
		uint8_t cmd[4];
	 
		/* Configure the command */
		cmd[0] = READ_CMD;
		cmd[1] = (uint8_t)(ReadAddr >> 16);
		cmd[2] = (uint8_t)(ReadAddr >> 8);
		cmd[3] = (uint8_t)(ReadAddr);
		
		W25Qx_Enable();
		/* Send the read ID command */
		SPI_Transmit(&hspi2, cmd, 4, W25Qx_TIMEOUT_VALUE);	
		/* Reception of the data */
		if (SPI_Receive(&hspi2, pData,Size,W25Qx_TIMEOUT_VALUE) != HAL_OK)
		{
				return W25Qx_ERROR;
		}
		W25Qx_Disable();
		return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 写数据
  * 输入参数: 缓存数组指针、写地址、字节数
  */
uint8_t bsp_w25q_write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
		uint8_t cmd[4];
		uint32_t end_addr, current_size, current_addr;
		uint32_t tickstart = HAL_GetTick();
		
		/* Calculation of the size between the write address and the end of the page */
		current_addr = 0;
	 
		while (current_addr <= WriteAddr)
		{
			current_addr += W25Q64FV_PAGE_SIZE;
		}
		current_size = current_addr - WriteAddr;
	 
		/* Check if the size of the data is less than the remaining place in the page */
		if (current_size > Size)
		{
			current_size = Size;
		}
	 
		/* Initialize the adress variables */
		current_addr = WriteAddr;
		end_addr = WriteAddr + Size;
		
		/* Perform the write page by page */
		do
		{
			/* Configure the command */
			cmd[0] = PAGE_PROG_CMD;
			cmd[1] = (uint8_t)(current_addr >> 16);
			cmd[2] = (uint8_t)(current_addr >> 8);
			cmd[3] = (uint8_t)(current_addr);
	 
			/* Enable write operations */
			bsp_w25q_writeEnable();
		
			W25Qx_Enable();
			/* Send the command */
			if (SPI_Transmit(&hspi2,cmd, 4, W25Qx_TIMEOUT_VALUE) != HAL_OK)
			{
				return W25Qx_ERROR;
			}
			
			/* Transmission of the data */
			if (SPI_Transmit(&hspi2, pData,current_size, W25Qx_TIMEOUT_VALUE) != HAL_OK)
			{
				return W25Qx_ERROR;
			}
				W25Qx_Disable();
				/* Wait the end of Flash writing */
			while(bsp_w25q_get_status() == W25Qx_BUSY);
			{
				/* Check for the Timeout */
				if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
				{        
					return W25Qx_TIMEOUT;
				}
			}
			
			/* Update the address and size variables for next page programming */
			current_addr += current_size;
			pData += current_size;
			current_size = ((current_addr + W25Q64FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q64FV_PAGE_SIZE;
		} while (current_addr < end_addr);
	 
		
		return W25Qx_OK;
}


 
 /**********************************************************************************
  * 函数功能: 唤醒
  */
void bsp_w25q_wakeup(void)   
{
	uint8_t cmd[] = {W25X_RPD_CMD};
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	SPI_Transmit(&hspi2, cmd, 1, W25Qx_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
}   


 
 /**********************************************************************************
  * 函数功能: 内存擦除(4KB每扇区，64KB每块)
  * 输入参数: 地址，擦除区域大小(sector/block)
  */
uint8_t bsp_w25q_erase(uint32_t Address, uint8_t Type)
{
		uint8_t cmd[4];
		uint32_t tickstart = HAL_GetTick();
		cmd[0] = Type;
		cmd[1] = (uint8_t)(Address >> 16);
		cmd[2] = (uint8_t)(Address >> 8);
		cmd[3] = (uint8_t)(Address);
		
		/* Enable write operations */
		bsp_w25q_writeEnable();
		
		/*Select the FLASH: Chip Select low */
		W25Qx_Enable();
		/* Send the read ID command */
		SPI_Transmit(&hspi2, cmd, 4, W25Qx_TIMEOUT_VALUE);	
		/*Deselect the FLASH: Chip Select high */
		W25Qx_Disable();
		
		/* Wait the end of Flash writing */
		while(bsp_w25q_get_status() == W25Qx_BUSY);
		{
				/* Check for the Timeout */
				if((HAL_GetTick() - tickstart) > W25Q64FV_SECTOR_ERASE_MAX_TIME)
				{        
						return W25Qx_TIMEOUT;
				}
		}
		return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 整片擦除
  */
uint8_t bsp_w25q_eraseChip(void)
{
		uint8_t cmd[4];
		uint32_t tickstart = HAL_GetTick();
		cmd[0] = CHIP_ERASE_CMD;
		
		/* Enable write operations */
		bsp_w25q_writeEnable();
		
		/*Select the FLASH: Chip Select low */
		W25Qx_Enable();
		/* Send the read ID command */
		SPI_Transmit(&hspi2, cmd, 1, W25Qx_TIMEOUT_VALUE);	
		/*Deselect the FLASH: Chip Select high */
		W25Qx_Disable();
		
		/* Wait the end of Flash writing */
		while(bsp_w25q_get_status() != W25Qx_BUSY);
		{
				/* Check for the Timeout */
				if((HAL_GetTick() - tickstart) > W25Q64FV_BULK_ERASE_MAX_TIME)
				{        
						return W25Qx_TIMEOUT;
				}
		}
		return W25Qx_OK;
}


//SPI总线写数据
static uint8_t SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	//尝试切换总线控制权
	bsp_spi_switch_to_w25q();
	//查询总线控制权
	if(!mutex_getDeviceAcc(&spi_mutex, W25QXX))
		return 1;
	HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//放弃总线控制权
	mutex_tryUnlock(&spi_mutex, W25QXX);
	return 0;
}
//SPI总线读数据
static uint8_t SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	bsp_spi_switch_to_w25q();
	if(!mutex_getDeviceAcc(&spi_mutex, W25QXX))
		return 1;
	HAL_SPI_Receive(hspi, pData, Size, Timeout);
	mutex_tryUnlock(&spi_mutex, W25QXX);
	return 0;
}
