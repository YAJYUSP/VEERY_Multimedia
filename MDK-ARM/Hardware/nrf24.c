#include "nrf24.h"
#include "spi.h"
#include "mutex.h"
#include <stdlib.h>
#include <main.h>
 
nrf_t nrf0;
const uint8_t NRF_ADDR[5] = {0x44,0x41,0x20,0x10,0x02};


//��spi���ߵ�ʹ��Ȩ�л���nrf24
uint8_t bsp_spi_switch_to_nrf24(void)
{
	//��ͼ����
	if(mutex_tryLock(&spi_mutex, NRF24))
		return 1;
	//�л������ٶ�
	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	HAL_SPI_Init(&hspi2);
}



//��ʼ��24L01��IO��
void bsp_nrf_init(void)
{
		memcpy(nrf0.RX_ADDRESS,NRF_ADDR,5);
		memcpy(nrf0.TX_ADDRESS,NRF_ADDR,5);
		NRF24L01_CE_LOW(); 			            			//ʹ��24L01
		NRF24L01_SPI_CS_DISABLE();			    			//SPIƬѡȡ��	 		 	 
}
 
 
/**
  * ��������: ���24L01�Ƿ����
  * �������: ��
  * �� �� ֵ: 0���ɹ�;1��ʧ��
  * ˵    ������          
  */ 
uint8_t bsp_nrf_check(void)
{
		uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
		uint8_t i;
		
		NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
		NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
		for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
		if(i!=5)return 1;//���24L01����	
		return 0;		 	//��⵽24L01
}	
 

 
/**
  * ��������: ����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: �������״��
  * ˵    ����txbuf:�����������׵�ַ
  *           
  */ 
uint8_t bsp_nrf_tx(uint8_t *txbuf)
{
		uint8_t sta;
		NRF24L01_CE_LOW();
		NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
		NRF24L01_CE_HIGH();//��������	 
		
		while(NRF24L01_IRQ_PIN_READ()!=0);//�ȴ��������
		
		sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		if(sta&MAX_TX)//�ﵽ����ط�����
		{
				NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
				return MAX_TX; 
		}
		if(sta&TX_OK)//�������
		{
				return TX_OK;
		}
		return 0xff;//����ԭ����ʧ��
}
 
/**
  * ��������:����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
uint8_t bsp_nrf_rx(uint8_t *rxbuf)
{
		uint8_t sta;		
		sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		if(sta&RX_OK)//���յ�����
		{
				NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
				NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
				return 0; 
		}	   
		return 1;//û�յ��κ�����
}			
 
/**
  * ��������: �ú�����ʼ��NRF24L01��RXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void bsp_nrf_rx_mode(void)
{
		NRF24L01_CE_LOW();	  
		NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC 
		NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
		NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
		NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
		
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
			
		NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)nrf0.RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
		
		NRF24L01_CE_HIGH(); //CEΪ��,�������ģʽ 
		HAL_Delay(1);
}	
 
/**
  * ��������: �ú�����ʼ��NRF24L01��TXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void bsp_nrf_tx_mode(void)
{														 
		NRF24L01_CE_LOW();	    
		NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)nrf0.TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
		NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)nrf0.RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
	 
		NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
		NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
		NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0xff);//�����Զ��ط����ʱ��:4000us + 86us;����Զ��ط�����:15��
		NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
		NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
		NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
		NRF24L01_CE_HIGH();//CEΪ��,10us����������
		HAL_Delay(1);
}
 
/**
  * ��������: �ú���NRF24L01����͹���ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */
void bsp_nrf_bypass_mode(void)
{
		NRF24L01_CE_LOW();	 
		NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x00);		//���ù���ģʽ:����ģʽ
}



/**
  * ��������: SPIд�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:ָ���Ĵ�����ַ
  *           
  */ 
static uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
		uint8_t status;	
		NRF24L01_SPI_CS_ENABLE();                 //ʹ��SPI����
		status =SPIx_ReadWriteByte(&hspi2,reg);   //���ͼĴ����� 
		SPIx_ReadWriteByte(&hspi2,value);         //д��Ĵ�����ֵ
		NRF24L01_SPI_CS_DISABLE();                //��ֹSPI����	   
		return(status);       			//����״ֵ̬
}
 
/**
  * ��������: ��ȡSPI�Ĵ���ֵ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:Ҫ���ļĴ���
  *           
  */ 
static uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
		uint8_t reg_val;	    
		NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����		
		SPIx_ReadWriteByte(&hspi2,reg);   //���ͼĴ�����
		reg_val=SPIx_ReadWriteByte(&hspi2,0XFF);//��ȡ�Ĵ�������
		NRF24L01_SPI_CS_DISABLE();          //��ֹSPI����		    
		return(reg_val);           //����״ֵ̬
}		
 
/**
  * ��������: ��ָ��λ�ö���ָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: �˴ζ�����״̬�Ĵ���ֵ 
  * ˵    ������
  * 
  */ 
static uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
		uint8_t status,uint8_t_ctr;	   
		
		NRF24L01_SPI_CS_ENABLE();           //ʹ��SPI����
		status=SPIx_ReadWriteByte(&hspi2,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
		for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
		{
				pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi2,0XFF);//��������
		}
		NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
		return status;        //���ض�����״ֵ̬
}
 
/**
  * ��������: ��ָ��λ��дָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:�Ĵ���(λ��)  *pBuf:����ָ��  len:���ݳ���
  *           
  */ 
static uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
		uint8_t status,uint8_t_ctr;	    
		NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����
		status = SPIx_ReadWriteByte(&hspi2,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
		for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
		{
				SPIx_ReadWriteByte(&hspi2,*pBuf++); //д������	 
		}
		NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
		return status;          //���ض�����״ֵ̬
}		

 
/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
static uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi,uint8_t byte)
{
		bsp_spi_switch_to_nrf24();
		if(!mutex_getDeviceAcc(&spi_mutex, NRF24))
			return 0xFF;
		uint8_t d_read,d_send=byte;
		if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
		{
			d_read=0xFF;
		}
		mutex_tryUnlock(&spi_mutex, NRF24);
		return d_read; 
}
