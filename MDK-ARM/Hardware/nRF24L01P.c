/**@file  	    nRF24L01P.c
* @brief            nRF24L01+ low level operations and configurations.
* @author           hyh
* @date             2021.9.17
* @version          1.0
* @copyright        Chengdu Ebyte Electronic Technology Co.Ltd
**********************************************************************************
*/
#include "nRF24L01P.h"
#include "spi.h"
#include "mutex.h"
#include <main.h>


/*the CE pin level status*/
static uint8_t CE_Status = 0;
//无线发送缓冲数组
uint8_t RF_SendBuffer[32] = {0};
//无线接收缓冲数组
uint8_t RF_ReceiveBuffer[32] = {0};


/*!
================================================================================
------------------------------------Functions-----------------------------------
================================================================================
*/


/*!
 *  @brief        Set the nRF24L01 into PowerDown mode          
 *  @param        None
 *  @return       None  
 *  @note          
*/
void bsp_l01_setPowerDown(void)
{
    uint8_t controlreg = bsp_l01_readSingleReg(L01REG_CONFIG);
    bsp_l01_writeSingleReg(L01REG_CONFIG,controlreg & (~(1 << PWR_UP)));
}
/*!
 *  @brief        Set the nRF24L01 into PowerUp mode       
 *  @param        None
 *  @return       None  
 *  @note          
*/
void bsp_l01_setPowerUp(void)
{
    uint8_t controlreg = bsp_l01_readSingleReg(L01REG_CONFIG);
    bsp_l01_writeSingleReg(L01REG_CONFIG,controlreg | (1 << PWR_UP));
}

/*!
 *  @brief        Write Transmit address into TX_ADDR register          
 *  @param        addrBuffer:the buffer stores the address
 *  @param        addr_size:the address byte num
 *  @return       None  
 *  @note         Used for a PTX device only 
*/
void bsp_l01_setTXAddr(uint8_t *addrBuffer,uint8_t addr_size)
{
    uint8_t size = (addr_size > 5) ? 5 : addr_size;
    bsp_l01_writeMultiReg(L01REG_TX_ADDR,addrBuffer,size);
}
/*!
 *  @brief        Write address for the RX pipe 
 *  @param        pipeNum:the number of the data pipe         
 *  @param        addrBuffer:the buffer stores the address
 *  @param        addr_size:the address byte num
 *  @return       None  
 *  @note          
*/
void bsp_l01_setRXAddr(uint8_t pipeNum,uint8_t *addrBuffer,uint8_t addr_size)
{
    uint8_t size = (addr_size > 5) ? 5 : addr_size;
    uint8_t num = (pipeNum > 5) ? 5 : pipeNum;
    bsp_l01_writeMultiReg(L01REG_RX_ADDR_P0 + num,addrBuffer,size);
}
/*!
 *  @brief        Set the data rate of the nRF24L01+          
 *  @param        drate:250K,1M,2M
 *  @return       None  
 *  @note          
*/
void bsp_l01_setDataRate(L01_DRATE drate)
{
    uint8_t mask = bsp_l01_readSingleReg(L01REG_RF_SETUP);
    mask &= ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));
    if(drate == DRATE_250K)
    {
        mask |= (1 << RF_DR_LOW);
    }
    else if(drate == DRATE_1M)
    {
        mask |= ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));
    }
    else if(drate == DRATE_2M)
    {
        mask |= (1 << RF_DR_HIGH);
    }
    bsp_l01_writeSingleReg(L01REG_RF_SETUP,mask);
}
/*!
 *  @brief        Set the power of the nRF24L01+          
 *  @param        power:18dB,12dB,6dB,0dB
 *  @return       None  
 *  @note          
*/
void bsp_l01_setPower(L01_PWR power)
{
    uint8_t mask = bsp_l01_readSingleReg(L01REG_RF_SETUP) & ~0x07;
    switch (power)
    {
    case POWER_N_18:
        mask |= PWR_N_18DB;
        break;
    case POWER_N_12:
        mask |= PWR_N_12DB;
        break;
    case POWER_N_6:
        mask |= PWR_N_6DB;
        break;
    case POWER_N_0:
        mask |= PWR_N_0DB;
        break;
    default:
        break;
    }
    bsp_l01_writeSingleReg(L01REG_RF_SETUP,mask);
}




/*!
 *  @brief        Switch the current RF mode to RX
 *  @param        None
 *  @return       None
 *  @note
*/
void bsp_l01_switchToRx(void)
{
    bsp_l01_setCE(CE_LOW);
    bsp_l01_setPowerUp();
    bsp_l01_setTRMode(RX_MODE);
    bsp_l01_setDataRate(DRATE_2M);
		bsp_l01_writeHoppingPoint(2);
		bsp_l01_flushRX();
		bsp_l01_flushTX();
		bsp_l01_clearIRQ(IRQ_ALL);
    bsp_l01_setCE(CE_HIGH);
}

/*!
 *  @brief         Switch the current RF mode to TX
 *  @param       None
 *  @return       None
 *  @note
*/
void bsp_l01_switchToTx(void)
{
    bsp_l01_setCE(CE_LOW);
    bsp_l01_setPowerUp();
    bsp_l01_setTRMode(TX_MODE);
    bsp_l01_setDataRate(DRATE_2M);
    bsp_l01_writeHoppingPoint(2);
		bsp_l01_flushRX();
		bsp_l01_flushTX();
    bsp_l01_clearIRQ(IRQ_ALL);
}

/**
  * 函数功能: 检测24L01是否存在
  * 输入参数: 无
  * 返 回 值: 0，成功;1，失败
  * 说    明：无          
  */ 
uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};

uint8_t bsp_l01_checkDevice(void)
{
		uint8_t i;
		
		bsp_l01_writeMultiReg(W_REGISTER+L01REG_TX_ADDR,buf,5);
		memset(buf,0x00,5);
		bsp_l01_readMultiReg(L01REG_TX_ADDR,buf,5);
		for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
		if(i!=5)return 1;//检测24L01错误	
		return 0;		 	//检测到24L01
}	


/*!
 *  @brief        Transmit handler for transmit mode
 *  @param        None
 *  @return       None
 *  @note
*/
uint8_t bsp_l01_transmitHandler(uint8_t *TxBuf, uint8_t *RxBuf)
{
		uint8_t len,IRQ_flag;
		uint8_t sta,timeout = 0;
		bsp_l01_switchToTx();
		bsp_l01_setCE(CE_LOW);
		bsp_l01_writeTXPayload_Ack(TxBuf, FIXED_PACKET_LEN);
		bsp_l01_setCE(CE_HIGH);
	
		while(NRF24L01_IRQ_PIN_READ() != 0)
		{
				HAL_Delay(1);
				timeout++;
				if(timeout >= 200)
						return 1;
		}
		IRQ_flag=bsp_l01_readIRQSource();//读取中断标志位
		if(IRQ_flag&0x20)								//判断中断是否为发送完成中断
		{
				len=bsp_l01_readRXPayload(RxBuf);//读取应答数据及长度
				return 0;
		}
		
		if(IRQ_flag&0x10)//判断是否为发送超时中断
		{
				RxBuf[0]=0xFF;
				len=1;
				return 1;
		}
		bsp_l01_flushTX();
		bsp_l01_flushRX();
		bsp_l01_clearIRQ(IRQ_ALL);
}
/*!
 *  @brief        Recieve handler for transmit mode and Returns the accepted quantity
 *  @param        None
 *  @return       None
 *  @note:       从手册上看，需要在发送机发送之前写入应答，接收机接受到数据后，会自动发生中断
 *               并将写好的带负载ACK数据返回给发送机；这里返回的即时接收数据量（最大32字节），
 *               故写在接收中断之后。
*/
void bsp_l01_recieveHandler(void)
{
    uint8_t len;
    if(NRF24L01_IRQ_PIN_READ() == 0)
    {
        if(bsp_l01_readIRQSource() & (1 << RX_DR))//detect RF module recieve interrupt
        {
            if((len = bsp_l01_readRXPayload(RF_ReceiveBuffer)) != 0)
            {
                bsp_l01_flushTX();//清空rf的发送fifo
                bsp_l01_writeRXPayload_InAck(&len,1);//写入应答数据,写入收到的数据量        
            }
        }
				bsp_l01_flushRX();
				bsp_l01_clearIRQ(IRQ_ALL);
    }
}


/*!
 *  @brief        Initialize the nRF24L01+         
 *  @param        None
 *  @return       None  
 *  @note          
*/
void bsp_l01_init(void)
{
    uint8_t addr[5] = {INIT_ADDR};
    uint8_t status = bsp_l01_getCEStatus();
    bsp_l01_setCE(CE_LOW);
    bsp_l01_setPowerDown();
    bsp_l01_clearIRQ(IRQ_ALL);
#if DYNAMIC_PACKET == 1
    //dynamic payload length
    bsp_l01_writeSingleReg(L01REG_DYNPD,(1 << DPL_P0));//Enable pipe 0 dynamic payload length
    bsp_l01_writeSingleReg(L01REG_FEATURE,(1 << EN_DPL)|(1 << EN_ACK_PAY));
#elif DYNAMIC_PACKET == 0
    //fixed payload length
    bsp_l01_writeSingleReg(L01REG_RX_PW_P0,FIXED_PACKET_LEN);
#endif
    bsp_l01_writeSingleReg(L01REG_CONFIG,(1 << EN_CRC));//Enable CRC,2 bytes
    bsp_l01_writeSingleReg(L01REG_ENAA,(1 << ENAA_P0));//Auto Ack in pipe0
    bsp_l01_writeSingleReg(L01REG_EN_RXADDR,(1 << ERX_P0));//Enable RX pipe 0
    bsp_l01_writeSingleReg(L01REG_SETUP_AW,AW_5BYTES);//Address width:5bytes
    bsp_l01_writeSingleReg(L01REG_SETUP_RETR,ARD_4000US|ARC_15);//ARD:4000us,repeat time:15
    bsp_l01_writeSingleReg(L01REG_RF_SETUP,(1 << RF_DR_LOW) | PWR_N_0DB);//Drate:250Kbps.Power:0dB
    bsp_l01_setDataRate(DRATE_2M);
    bsp_l01_setTXAddr(addr,5);//Set TX address
    bsp_l01_setRXAddr(0,addr,5);//Set RX address
    bsp_l01_setTRMode(RX_MODE);
    bsp_l01_writeHoppingPoint(2);
    bsp_l01_setPowerUp();
}


/***************************************************************************************************************************/


/*!
 *  @brief        Set the frequency of the nRF24L01+          
 *  @param        freq:the hopping frequency point,range:0-125,2400Mhz-2525Mhz
 *  @return       None  
 *  @note          
*/
static void bsp_l01_writeHoppingPoint(uint8_t freq)
{
    bsp_l01_writeSingleReg(L01REG_RF_CH,freq <= 125 ? freq : 125);
}


/*!
 *  @brief        NOP operation about the nRF24L01+           
 *  @param        None
 *  @return       None  
 *  @note          
*/
static void bsp_l01_nop(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(NOP);
    L01_CSN_HIGH();
}
/*!
 *  @brief        Read the status register of the nRF24L01+           
 *  @param        None
 *  @return       the value of the status register  
 *  @note          
*/
static uint8_t bsp_l01_readStatusReg(void)
{
    uint8_t status;
    L01_CSN_LOW();
    status = SPI_ExchangeByte(R_REGISTER + L01REG_STATUS);
    L01_CSN_HIGH();
    return status;
}
/*!
 *  @brief        Clear the IRQ caused by the nRF24L01+           
 *  @param        irqMask:RX_DR(bit[6]),TX_DS(bit[5]),MAX_RT(bit[4])
 *  @return       None  
 *  @note          
*/
static void bsp_l01_clearIRQ(uint8_t irqMask)
{
    uint8_t status = 0;
    irqMask &= IRQ_ALL;
    status = bsp_l01_readStatusReg();
    L01_CSN_LOW();
    bsp_l01_writeSingleReg(L01REG_STATUS,irqMask | status);
    L01_CSN_HIGH();
    bsp_l01_readStatusReg();
}
/*!
 *  @brief        Read the IRQ status of the nRF24L01+           
 *  @param        None
 *  @return       irqMask:RX_DR(bit[6]),TX_DS(bit[5]),MAX_RT(bit[4]) 
 *  @note          
*/
static uint8_t bsp_l01_readIRQSource(void)
{
    uint8_t status = 0;
    status = bsp_l01_readStatusReg();
    return (status & IRQ_ALL);
}
/*!
 *  @brief        Read the payload width of the top buffer of the FIFO           
 *  @param        None
 *  @return       width:the width of the pipe buffer  
 *  @note          
*/
static uint8_t bsp_l01_readTopFIFOWidth(void)
{
    uint8_t width;
    L01_CSN_LOW();
    SPI_ExchangeByte(R_RX_PL_WID);
    width = SPI_ExchangeByte(NOP);
    L01_CSN_HIGH();
    return width;
}
/*!
 *  @brief        Read the RX payload from the FIFO and store them in buffer            
 *  @param        buffer:the buffer to store the data
 *  @return       the length to be read
 *  @note          
*/
static uint8_t bsp_l01_readRXPayload(uint8_t *buffer)
{
    uint8_t width,i;
    width = bsp_l01_readTopFIFOWidth();
    if(width > 32)
    {
        L01_CSN_HIGH();
        bsp_l01_flushRX();
        return 0;
    }
    L01_CSN_LOW();
    SPI_ExchangeByte(R_RX_PAYLOAD);
    for (i = 0; i < width; i++)
    {
        *(buffer + i) = SPI_ExchangeByte(NOP);
    }
    L01_CSN_HIGH();
    bsp_l01_flushRX();
    return width;
}
/*!
 *  @brief        Write TX Payload to a data pipe,and PRX will return ACK back         
 *  @param        buffer:the buffer stores the data
 *  @param        size:the size to be written  
 *  @return       None  
 *  @note          
*/
static void bsp_l01_writeTXPayload_Ack(uint8_t *buffer,uint8_t size)
{
		uint8_t i;
    uint8_t w_size = (size > 32) ? 32 : size;
    bsp_l01_flushTX();
		L01_CSN_LOW();
    SPI_ExchangeByte(W_TX_PAYLOAD);
    for (i = 0; i < w_size; i++)
    {
        SPI_ExchangeByte(*(buffer + i));
    }
    L01_CSN_HIGH();
}
/*!
 *  @brief        Write TX Payload to a data pipe,and PRX won't return ACK back         
 *  @param        buffer:the buffer stores the data
 *  @param        size:the size to be written  
 *  @return       None  
 *  @note          
*/
static void bsp_l01_writeTXPayload_NoAck(uint8_t *buffer,uint8_t size)
{
    if (size > 32 || size == 0)
    {
        return;
    }
    L01_CSN_LOW();
    SPI_ExchangeByte(W_TX_PAYLOAD_NOACK);
    while (size --)
    {
        SPI_ExchangeByte(*buffer++);
    }
    L01_CSN_HIGH();
}
/*!
 *  @brief        Write TX Payload to a data pipe when RX mode         
 *  @param        buffer:the buffer stores the data
 *  @param        size:the size to be written  
 *  @return       None  
 *  @note          
*/
static void bsp_l01_writeRXPayload_InAck(uint8_t *buffer,uint8_t size)
{
    uint8_t i;
    uint8_t w_size = (size > 32) ? 32 : size;
    L01_CSN_LOW();
    SPI_ExchangeByte(W_ACK_PAYLOAD);
    for (i = 0; i < w_size; i++)
    {
        SPI_ExchangeByte(*(buffer + i));
    }
    L01_CSN_HIGH();
}


/**
  * 函数功能: 往SPI读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
static uint8_t SPI_ExchangeByte(uint8_t byte)
{
	bsp_spi_switch_to_nrf24();
	if(!mutex_getDeviceAcc(&spi_mutex, NRF24))
		return 0xFF;
	uint8_t d_read,d_send=byte;
	if(HAL_SPI_TransmitReceive(&hspi2,&d_send,&d_read,1,0xFF)!=HAL_OK)
	{
		d_read=0xFF;
	}
	mutex_tryUnlock(&spi_mutex, NRF24);
	return d_read; 
}




/*!
 *  @brief          Get the level status of the CE pin
 *  @param          None     
 *  @return         CE pin level status: 0:low; 1:high
 *  @note          
*/
static uint8_t bsp_l01_getCEStatus(void)
{
    return CE_Status;
}
/*!
 *  @brief          Set the level status of the CE pin low or high
 *  @param          status:CE pin level status    
 *  @return         None
 *  @note          
*/
static void bsp_l01_setCE(CE_STAUS status)
{
    CE_Status = status;
    if (status == CE_LOW)    { L01_CE_LOW();}
    else                     { L01_CE_HIGH(); }
}
/*!
 *  @brief        Read the value from the specified register   
 *  @param        addr:the address of the register
 *  @return       value:the value read from the register  
 *  @note          
*/
static uint8_t bsp_l01_readSingleReg(uint8_t addr)
{
    uint8_t value;
    L01_CSN_LOW();
    SPI_ExchangeByte( R_REGISTER | addr);
    value = SPI_ExchangeByte(NOP);
    L01_CSN_HIGH();
    return value;
}
/*!
 *  @brief        Read the values of the specified registers and store them in buffer
 *  @param        start_addr:the start address of the registers
 *  @param        buffer:the buffer stores the read values
*  @param         size:the size to be read
 *  @return       None  
 *  @note          
*/
static void bsp_l01_readMultiReg(uint8_t start_addr,uint8_t *buffer,uint8_t size)
{
    uint8_t i;
    L01_CSN_LOW();
    SPI_ExchangeByte(R_REGISTER | start_addr);
    for (i = 0; i < size; i++)
    {
        *(buffer + i) = SPI_ExchangeByte(NOP);
    }
    L01_CSN_HIGH();
}
/*!
 *  @brief        Write a value to the specified register   
 *  @param        addr:the address of the register
 *  @param        value:the value to be written  
 *  @return       None
 *  @note          
*/
static void bsp_l01_writeSingleReg(uint8_t addr,uint8_t value)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(W_REGISTER | addr);
    SPI_ExchangeByte(value);
    L01_CSN_HIGH();
}
/*!
 *  @brief        Write buffer to the specified registers  
 *  @param        start_addr:the start address of the registers
 *  @param        buffer:the buffer to be written
 *  @param        size:the size to be written  
 *  @return       None
 *  @note          
*/
static void bsp_l01_writeMultiReg(uint8_t start_addr,uint8_t *buffer,uint8_t size)
{
    uint8_t i;
    L01_CSN_LOW();
    SPI_ExchangeByte(W_REGISTER | start_addr);
    for ( i = 0; i < size; i++)
    {
        SPI_ExchangeByte(*(buffer + i));
    }
    L01_CSN_HIGH();
}

/*!
 *  @brief        Flush the TX buffer             
 *  @param        None 
 *  @return       None  
 *  @note          
*/
static void bsp_l01_flushTX(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(FLUSH_TX);
    L01_CSN_HIGH();
}
/*!
 *  @brief        Flush the RX buffer           
 *  @param        None
 *  @return       None  
 *  @note          
*/
static void bsp_l01_flushRX(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(FLUSH_RX);
    L01_CSN_HIGH();
}


//将spi总线的使用权切换到nrf24
static uint8_t bsp_spi_switch_to_nrf24(void)
{
	//试图上锁
	if(mutex_tryLock(&spi_mutex, NRF24))
		return 1;
	//切换总线速度
	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	HAL_SPI_Init(&hspi2);
}

/*!
 *  @brief        Reuse the last transmitted payload           
 *  @param        None
 *  @return       None  
 *  @note          
*/
static void bsp_l01_reuseTXPayload(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(REUSE_TX_PL);
    L01_CSN_HIGH();
}

/*!
 *  @brief        Set the nRF24L01+ as TX/RX mode         
 *  @param        mode:TX/RX
 *  @return       None  
 *  @note          
*/
static void bsp_l01_setTRMode(L01_MODE mode)
{
    uint8_t mask = bsp_l01_readSingleReg(L01REG_CONFIG);
    if (mode == TX_MODE)
    {
        mask &= ~(1 << PRIM_RX);
    }
    else if (mode == RX_MODE)
    {
        mask |= (1 << PRIM_RX);
    }
    bsp_l01_writeSingleReg(L01REG_CONFIG,mask);
}
