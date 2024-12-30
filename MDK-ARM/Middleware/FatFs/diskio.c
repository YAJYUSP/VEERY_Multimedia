/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "w25q64.h"
#include "ffconf.h"


/* Ϊÿ���豸����һ�������� */
#define ATA			    0     // Ԥ��SD��ʹ��
#define SPI_FLASH		1     // �ⲿSPI Flash

/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{

	DSTATUS status = STA_NOINIT;
	
	uint8_t ID[4];  //�豸ID��������
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			break;
    
		case SPI_FLASH:      
      /* SPI Flash״̬��⣺��ȡSPI Flash �豸ID */
			bsp_w25q_readID(ID);
      if(ID[1] == FLASH_ID)
      {
        /* �豸ID��ȡ�����ȷ */
        status = RES_OK;
      }
			break;
		default:
			break;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
)
{
  uint16_t i;
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case ATA:	         /* SD CARD */
			break;
    
		case SPI_FLASH:    /* SPI Flash */ 
			/* ��ʼ��SPI Flash�Ѿ����ⲿ���� */
      /* ��ȡSPI FlashоƬ״̬ */
      status=disk_status(SPI_FLASH);
			break;
      
		default:
			status = STA_NOINIT;
	}
	return status;
}


/*-----------------------------------------------------------------------*/
/* ����������ȡ�������ݵ�ָ���洢��                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* �豸������(0..) */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	UINT count		/* ��������(1..128) */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			break;
    
		case SPI_FLASH:
      bsp_w25q_read(buff, sector<<12, count<<12);
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* д������������д��ָ�������ռ���                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			  /* �豸������(0..) */
	const BYTE *buff,	/* ��д�����ݵĻ����� */
	DWORD sector,		  /* �����׵�ַ */
	UINT count			  /* ��������(1..128) */
)
{
  uint32_t write_addr; 
	DRESULT status = RES_PARERR;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case ATA:	/* SD CARD */      
		break;

		case SPI_FLASH:
      write_addr = sector<<12;    
      bsp_w25q_erase(write_addr,SECTOR_ERASE_CMD);
      bsp_w25q_write((uint8_t *)buff,write_addr,count<<12);
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* ��������                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* ������ */
	BYTE cmd,		  /* ����ָ�� */
	void *buff		/* д����߶�ȡ���ݵ�ַָ�� */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			break;
    
		case SPI_FLASH:
			switch (cmd) {
        /* ����������1536*4096/1024/1024=6(MB) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 2048;		
        break;
        /* ������С  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096;
        break;
        /* ͬʱ������������ */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
        break;        
      }
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

__weak DWORD get_fattime(void) {
	/* ���ص�ǰʱ��� */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

