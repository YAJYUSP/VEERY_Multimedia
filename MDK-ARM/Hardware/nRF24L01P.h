/**@file  	    nRF24L01P.h
* @brief            nRF24L01+ low level operations and configurations.
* @author           hyh
* @date             2021.6.9
* @version          1.0
* @copyright        Chengdu Ebyte Electronic Technology Co.Ltd
**********************************************************************************
*/
#ifndef nRF24L01P_H
#define nRF24L01P_H

//#include "bsp.h"
#include "stdint.h"
#include "nRF24L01P_REG.h"


#define IRQ_ALL ((1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT))
/*Data Rate selection*/
typedef enum {DRATE_250K,DRATE_1M,DRATE_2M}L01_DRATE;
/*Power selection*/
typedef enum {POWER_N_0,POWER_N_6,POWER_N_12,POWER_N_18}L01_PWR;
/*Mode selection*/
typedef enum {TX_MODE,RX_MODE}L01_MODE;
/*CE pin level selection*/
typedef enum {CE_LOW,CE_HIGH}CE_STAUS;

/*Select whether to use ACK mechanism for sending 
nrf_init_flag  : 1--->ACK
                 0--->NO_ACK
*/
#define nrf_init_flag   1

 
#define L01_CSN_LOW()                      HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET)
#define L01_CSN_HIGH()                     HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET)
 
#define L01_CE_LOW()                             HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)
#define L01_CE_HIGH()                            HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)

#define NRF24L01_IRQ_PIN_READ()                       HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port,NRF_IRQ_Pin)



/*
================================================================================
============================Configurations and Options==========================
================================================================================
*/
#define DYNAMIC_PACKET      1 //1:DYNAMIC packet length, 0:fixed
#define FIXED_PACKET_LEN    32//Packet size in fixed size mode
#define INIT_ADDR           0x44,0x41,0x20,0x10,0xFF	


/*
================================================================================
-------------------------------------Exported APIs------------------------------
================================================================================
*/
static void bsp_l01_setTRMode(L01_MODE mode);
static void bsp_l01_reuseTXPayload(void);
static uint8_t bsp_spi_switch_to_nrf24(void);
static void bsp_l01_flushRX(void);
static void bsp_l01_flushTX(void);
static void bsp_l01_writeMultiReg(uint8_t start_addr,uint8_t *buffer,uint8_t size);
static void bsp_l01_writeSingleReg(uint8_t addr,uint8_t value);
static void bsp_l01_readMultiReg(uint8_t start_addr,uint8_t *buffer,uint8_t size);
static uint8_t bsp_l01_readSingleReg(uint8_t addr);
static void bsp_l01_setCE(CE_STAUS status);
static uint8_t bsp_l01_getCEStatus(void);
static uint8_t SPI_ExchangeByte(uint8_t byte);
static void bsp_l01_writeRXPayload_InAck(uint8_t *buffer,uint8_t size);
static void bsp_l01_writeTXPayload_NoAck(uint8_t *buffer,uint8_t size);
static void bsp_l01_writeTXPayload_Ack(uint8_t *buffer,uint8_t size);
static uint8_t bsp_l01_readRXPayload(uint8_t *buffer);
static uint8_t bsp_l01_readTopFIFOWidth(void);
static uint8_t bsp_l01_readIRQSource(void);
static void bsp_l01_clearIRQ(uint8_t irqMask);
static uint8_t bsp_l01_readStatusReg(void);
static void bsp_l01_nop(void);
static void bsp_l01_writeHoppingPoint(uint8_t freq);


void bsp_l01_init(void);
void bsp_l01_recieveHandler(void);
uint8_t bsp_l01_transmitHandler(uint8_t *TxBuf, uint8_t *RxBuf);
uint8_t bsp_l01_checkDevice(void);
void bsp_l01_switchToTx(void);
void bsp_l01_switchToRx(void);
void bsp_l01_setPower(L01_PWR power);
void bsp_l01_setDataRate(L01_DRATE drate);
void bsp_l01_setRXAddr(uint8_t pipeNum,uint8_t *addrBuffer,uint8_t addr_size);
void bsp_l01_setTXAddr(uint8_t *addrBuffer,uint8_t addr_size);
void bsp_l01_setPowerUp(void);
void bsp_l01_setPowerDown(void);


#endif
