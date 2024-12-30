/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "lcd.h"
#include "key.h"
#include "timer.h"
#include "encoder.h"
#include "usb_switch.h"
#include "mpu6050.h"
#include "w25q64.h"
#include "nRF24L01P.h"
#include "extio.h"
#include "drv2605l.h"
#include "uart_drv.h"


#include "ff.h"
#include "diskio.h"
#include "shell.h"

#include "string.h"
#include "stdlib.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t LCD_GRAM[32768] = {0};//32768Bytes
uint8_t LCD_GRAM[8] = {0};

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT f_res;     					  /* �ļ��ɹ���д���� */
char fpath[100];                  /* ���浱ǰɨ��·�� */
char readbuffer[512];   

//**
//  * �ļ���Ϣ��ȡ
//  */
FILINFO fno;
static FRESULT file_check(void)
{
  
  /* ��ȡ�ļ���Ϣ */
  f_res=f_stat("1:TEST.txt",&fno);
  if(f_res==FR_OK)
  {
    printf("��1:TEST.txt���ļ���Ϣ��\n");
    printf("�ļ���С: %ld(�ֽ�)\n", fno.fsize);
    printf("ʱ���: %u/%02u/%02u, %02u:%02u\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("����: %c%c%c%c%c\n\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',      // ��һ��Ŀ¼
           (fno.fattrib & AM_RDO) ? 'R' : '-',      // ֻ���ļ�
           (fno.fattrib & AM_HID) ? 'H' : '-',      // �����ļ�
           (fno.fattrib & AM_SYS) ? 'S' : '-',      // ϵͳ�ļ�
           (fno.fattrib & AM_ARC) ? 'A' : '-');     // �����ļ�
  }
	else
		printf("��ȡ�ļ���Ϣʧ�ܣ�����ֵΪ%d\n", f_res);
  return f_res;
}



/* FatFs����ܲ��� */
static FRESULT miscellaneous(void)
{
  DIR dir;
  FATFS *pfs;
  DWORD fre_clust, fre_sect, tot_sect;
	UINT bw;          // ʵ��д����ֽ���
	UINT br;          // ʵ�ʶ�ȡ���ֽ���

  /* ��ȡ�豸��Ϣ�Ϳմش�С */
  f_res = f_getfree("1:", &fre_clust, &pfs);

  /* ����õ��ܵ����������Ϳ��������� */
  tot_sect = (pfs->n_fatent - 2) * pfs->csize;
  fre_sect = fre_clust * pfs->csize;

  /* ��ӡ��Ϣ(4096 �ֽ�/����) */
  printf("\n�豸�ܿռ䣺%10lu KB��\n���ÿռ䣺  %10lu KB��\n", tot_sect *4, fre_sect *4);
  
	
  printf("\n�����ļ���д��...\r\n");
  f_res = f_open(&fnew, "1:TEST.txt",
                             FA_CREATE_ALWAYS | FA_WRITE);
	if ( f_res == FR_OK )
	{
		printf("���ļ��ɹ�\r\n");
    /* ��λ���ļ�ĩ */
    f_res = f_lseek(&fnew,f_size(&fnew));
    if (f_res == FR_OK)
    {
      /* ��ʽ��д�룬������ʽ����printf���� */
      f_printf(&fnew,"��ã�������1:TEST.txt�ļ������ݡ�");
			printf("����д��\r\n");
    }
    f_close(&fnew);    
	}
  else
  {
    printf("���ļ�ʧ�ܣ�%d\n\n",f_res);
  }
	
	HAL_Delay(10);
	
	printf("\n���Զ�ȡ�ļ�...\r\n");
	f_res = f_open(&fnew, "1:TEST.txt",
                            FA_OPEN_ALWAYS|FA_READ);
	if ( f_res == FR_OK )
	{
		printf("���ļ��ɹ�\r\n");
		/*  �ļ���λ���ļ���ʼλ�� */
		f_res = f_lseek(&fnew,0);
		/* ��ȡ�ļ��������ݵ������� */
		f_res = f_read(&fnew,readbuffer,f_size(&fnew),&br);
		if(f_res == FR_OK)
		{
			printf("�ļ����ݣ�\n%s",readbuffer);
			printf("\n�ļ�����\n");
			memset(readbuffer, 0, 7);
		}
    f_close(&fnew);    
	}
  else
  {
    printf("���ļ�ʧ�ܣ�%d\n",f_res);
  }
	
  return f_res;
}



/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&path[i], "/%s", fn); 		
        //�ݹ����         
        res = scan_files(path);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);								//����ļ���	
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  } 
  return res; 
}




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//debug��Ϣ��ӡ��shell0��
void DEBUG_OUT(char *fmt, ...)
{
	shellPrint(&shell0, fmt);
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM9_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	bsp_extio_init();
	shell_init();
	
//	LCD_Init();
//	HAL_Delay(10);
//	LCD_Fill_GRAM(1, 1, 127, 127, WHITE);
//	LCD_Reff_reshGRAM();
//	bsp_lcd_bl_pwm_set(20);

	while(bsp_mpu6050_init())
	{ 
		HAL_Delay(10);
	}

	bsp_l01_init();
	while(bsp_l01_checkDevice())
	{
		HAL_Delay(10);
	} 
	bsp_l01_switchToTx();
	
//	bsp_drv2605l_init();
//	bsp_drv2605l_set_waveform(0, 51);
//	bsp_drv2605l_go();
//	bsp_drv2605l_set_waveform(0, 26);
	
	bsp_encoder_init();
	bsp_tim11_5ms_isr_init();
	
	bsp_w25q_init();
	
	//���ⲿSPI Flash�����ļ�ϵͳ
//	f_res = f_mount(&fs,"1:",1);
//  if(f_res!=FR_OK)
//  {
//    DEBUG_OUT("�ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",f_res);
//		while(1);
//  }
//  else
//  {
//    DEBUG_OUT("�ļ�ϵͳ���سɹ������Խ��в���\r\n");    
//  }

	DEBUG_OUT("USB Initializing...\r\n");
	MX_USB_DEVICE_Init();
	HAL_Delay(1000);
	DEBUG_OUT("switching Type-C port to USBMSC...\r\n");
	bsp_typec_usb_func_switch(USB);

//	f_mkfs("1:", 0, 4096);
//	DEBUG_OUT("mkfs:%d\r\n\r\n", f_res);
	                               
	
//	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
//	f_res = f_mount(&fs,"1:",1);
//  if(f_res!=FR_OK)
//  {
//    DEBUG_OUT("�ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",f_res);
//		while(1);
//  }
//  else
//  {
//    DEBUG_OUT("�ļ�ϵͳ���سɹ������Խ��в���\r\n");    
//  }
//	
//	DEBUG_OUT("***************** �ļ�ɨ����� ****************\r\n");
//  strcpy(fpath,"1:");
//  scan_files(fpath);
//  
//  /* FatFs����ܲ��� */
//  f_res = miscellaneous();

//  DEBUG_OUT("\n*************** �ļ���Ϣ��ȡ���� **************\r\n");
//  f_res = file_check();

//  DEBUG_OUT("***************** �ļ�ɨ����� ****************\r\n");
//  strcpy(fpath,"1:");
//  scan_files(fpath);
//  
//	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
//	f_mount(NULL,"1:",1);
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: DEBUG_OUT("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
