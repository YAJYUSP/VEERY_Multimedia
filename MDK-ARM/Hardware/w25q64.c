#include "w25q64.h"
#include "mutex.h"
#include <stdlib.h>
#include <stdio.h>
#include <main.h>
#include <spi.h>



 //��spi���ߵ�ʹ��Ȩ�л���w25qxx
uint8_t bsp_spi_switch_to_w25q(void)
{
	//��ͼ����
	if(mutex_tryLock(&spi_mutex, W25QXX))
		return 1;
	//�л������ٶ�
	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	HAL_SPI_Init(&hspi2);
}


 
 /**********************************************************************************
  * ��������: ģ���ʼ��
  */
uint8_t bsp_w25q_init(void)
{ 	
		bsp_w25q_reset();	
	 /* ��ʱһС��ʱ�� */
		HAL_Delay(10);
		/* ����SPI Flash */
		bsp_w25q_wakeup();
		/* ��ȡSPI FlashоƬ״̬ */
		return bsp_w25q_get_status();
}



/**num:0~n*******/
void W25Q64_WriteImg(uint8_t num ,uint32_t *ImgBuf)
{
		printf("\r\n SPI-W25Qxx writing img \r\n\r\n");
		uint32_t ImgAddr = PIC_DATA_ADDR_BASE + num * HBLOCK_SIZE +1;
		/*-Step2- ������  ************************************************Step2*/ 	
		if(bsp_w25q_erase(ImgAddr,HBLOCK_ERASE_CMD) == W25Qx_OK)
			printf(" SPIFlash erase block OK!\r\n");
		else
			printf("something wrong in erasing\r\n");
		/*-Step3- д����  ************************************************Step3*/	
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
		uint8_t wData[0x100];   //д��������
		uint8_t rData[0x100];   //����������
		uint8_t ID[4];          //�豸ID��������
		uint32_t i;

		printf("\r\n SPI-W25Qxx Self Check \r\n\r\n");
		/*-Step1- ��֤�豸ID  ************************************************Step1*/ 
		bsp_w25q_readID(ID);
		 //��һλ����ID�̶�0xEF,�ڶ�λ�豸ID����������ͬ,����Ϊ��
		 //W25Q16Ϊ0x14��32Ϊ0x15��40Ϊ0x12��64Ϊ0x16��80Ϊ0x13��128Ϊ0x17
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
		/*-Step2- ������  ************************************************Step2*/ 	
		if(bsp_w25q_erase(0,HBLOCK_ERASE_CMD) == W25Qx_OK)
				printf(" SPI Erase Block OK!\r\n");
		else
				printf("something wrong in Erasing\r\n");
		/*-Step3- д����  ************************************************Step3*/	
		for(i =0;i<0x100;i ++)
		{
				wData[i] = i;
				rData[i] = 0;
		}
		if(bsp_w25q_write(wData,0x00,0x100)== W25Qx_OK)
				printf(" SPI Write data OK!\r\n");
		else
				printf("something wrong in Writing\r\n");
			/*-Step4- ������  ************************************************Step4*/	
		if(bsp_w25q_read(rData,0x00,0x100)== W25Qx_OK)
				printf(" SPI Read data ok\r\n\r\n");
		else
				printf("something wrong in Reading\r\n");
		
		printf("SPI Read Data : \r\n");
		for(i =0;i<0x100;i++)
				printf("0x%02X  ",rData[i]);
		printf("\r\n\r\n");
		/*-Step5- ���ݶԱ�  ************************************************Step5*/		
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
  * ��������: ��ȡ�豸״̬
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
  * ��������: дʹ��
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
  * ��������: ��ȡ�豸ID
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
  * ��������: ������
  * �������: ��������ָ�롢����ַ���ֽ���
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
  * ��������: д����
  * �������: ��������ָ�롢д��ַ���ֽ���
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
  * ��������: ����
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
  * ��������: �ڴ����(4KBÿ������64KBÿ��)
  * �������: ��ַ�����������С(sector/block)
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
  * ��������: ��Ƭ����
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


//SPI����д����
static uint8_t SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	//�����л����߿���Ȩ
	bsp_spi_switch_to_w25q();
	//��ѯ���߿���Ȩ
	if(!mutex_getDeviceAcc(&spi_mutex, W25QXX))
		return 1;
	HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//�������߿���Ȩ
	mutex_tryUnlock(&spi_mutex, W25QXX);
	return 0;
}
//SPI���߶�����
static uint8_t SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	bsp_spi_switch_to_w25q();
	if(!mutex_getDeviceAcc(&spi_mutex, W25QXX))
		return 1;
	HAL_SPI_Receive(hspi, pData, Size, Timeout);
	mutex_tryUnlock(&spi_mutex, W25QXX);
	return 0;
}
