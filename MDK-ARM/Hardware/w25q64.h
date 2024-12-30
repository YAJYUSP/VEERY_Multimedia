#ifndef __W25Q64_H
#define __W25Q64_H
 
#include <stdint.h>
#include <spi.h>

#define FLASH_ID 0x16


/* Includes ------------------------------------------------------------------*/

#define W25Q64FV_FLASH_SIZE                   0x800000 /* 64 MBits => 8MBytes */
#define W25Q64FV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q64FV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q64FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */
 
#define W25Q64FV_DUMMY_CYCLES_READ           4
#define W25Q64FV_DUMMY_CYCLES_READ_QUAD      10
 
#define W25Q64FV_BULK_ERASE_MAX_TIME         250000
#define W25Q64FV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q64FV_SUBSECTOR_ERASE_MAX_TIME    800
#define W25Qx_TIMEOUT_VALUE 1000
 
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99
 
#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF
 
/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F
 
/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB
 
/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Power Down Options */
#define W25X_PD_CMD			      0xB9 
#define W25X_RPD_CMD	    0xAB 
 
/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15
 
#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11
 
 
/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
 
 
/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7
#define BLOCK_ERASE_CMD                      0xD8
#define HBLOCK_ERASE_CMD                     0x52
 
#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75
 
 
/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
 
 
#define W25Qx_Enable() 			HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_RESET)
#define W25Qx_Disable() 		HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_SET)
 
#define W25Qx_OK            ((uint8_t)0x00)
#define W25Qx_ERROR         ((uint8_t)0x01)
#define W25Qx_BUSY          ((uint8_t)0x02)
#define W25Qx_TIMEOUT		((uint8_t)0x03)

//图片存储区基地址:从 第65 BLOCK开始
#define PIC_DATA_ADDR_BASE 65 * 2 * HBLOCK_SIZE 
//半BLOCK的大小：32KBytes
#define HBLOCK_SIZE 32768 
//扇区大小：4KBytes
#define SECTOR_SIZE 4096

//掉电存储区基地址
#define POS_DATA_ADDR_BASE 65536 
#define POS_DATA_LEN 64//UNIT:Bytes
 

uint8_t bsp_spi_switch_to_w25q(void);
uint8_t bsp_w25q_init(void);
uint8_t bsp_w25q_eraseChip(void);
uint8_t bsp_w25q_erase(uint32_t Address, uint8_t Type);
uint8_t bsp_w25q_write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t bsp_w25q_read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
void bsp_w25q_readID(uint8_t *ID);
uint8_t bsp_w25q_writeEnable(void);
uint8_t bsp_w25q_get_status(void);
void	bsp_w25q_reset(void);
void bsp_w25q_wakeup(void);
void bsp_w25q_test(void);
void W25Q64_WriteImg(uint8_t num ,uint32_t *ImgBuf);
void w25q_read_pic_to_GRAM(uint8_t num);



static uint8_t SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static uint8_t SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);

 
#endif 

