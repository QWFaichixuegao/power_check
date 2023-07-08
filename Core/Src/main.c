/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_read.h"
#include "at24cxx.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char buf[5];
char bufadd[128];
uint8_t testcount=0;
uint8_t turnledflag=0;
uint8_t turnledflag2=0;
uint16_t statecnt=0;
uint16_t ledcnt=0;

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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart1,usart1_handle.rx_buf,RX_SIZE);  
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	adc_init(&read_adc);
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&read_adc.adc_value,2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		state_change();
//		HAL_UART_Transmit_DMA(&huart1,(uint8_t*)buf,strlen(buf));	
		HAL_Delay(10);
		if(read_adc.usb > 1.5)
			{
				if(turnledflag == 1)
					{
						HAL_GPIO_WritePin(GPIOA, LED_R_Pin|LED_B_Pin, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOA, SWITCH1_Pin|SWITCH2_Pin, GPIO_PIN_RESET);//接上USB时关闭红外灯 GPIO_PIN_RESET
						handle.usbflag = ENABLE;
						turnledflag = 0;
					}
			}
		else
			{
				if(turnledflag == 0)
					{
						HAL_GPIO_WritePin(GPIOA, SWITCH1_Pin|SWITCH2_Pin, GPIO_PIN_RESET);//开启红外光//未接上USB，测量时打开红外灯 GPIO_PIN_SET
						handle.usbflag = DISABLE;
						turnledflag = 1;
					}
				ledcnt++;
				if(ledcnt == 100)
					{
						if(turnledflag2 == 0)
							{
								HAL_GPIO_WritePin(GPIOA, LED_B_Pin, GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
								turnledflag2=1;
							}
						else
							{
								HAL_GPIO_WritePin(GPIOA, LED_B_Pin, GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
								turnledflag2=0;
							}
						ledcnt=0;
					}	
			}
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
 uint8_t  cleancount[2]={0,0};
/* USER CODE BEGIN 4 */
void state_change(void)
{
	read_adc.usb	  = read_adc.adc_value[1]*3.3f/4096;
	switch (handle.flagstate)
		{
			case SAVE://1
				if(handle.usbflag == DISABLE)
				{  
					statecnt++;
					if(statecnt == 500)
					{
						if(saveadc_data.data_count<13000)
						{
							read_adc.charge = read_adc.adc_value[0]*3.3f/4096;
							sprintf((char *)saveadc_data.at24_w_data,"%.2f,",read_adc.charge);
							AT24CXX_WriteData(0x0003+saveadc_data.data_count*5, saveadc_data.at24_w_data,5);//前三个字节是采样总个数
							
//							sprintf((char *)saveadc_data.at24_w_data,"%d,",read_adc.adc_value[0]);//直接存4位整数（满值是4096）
//							AT24CXX_WriteData(0x0003+saveadc_data.data_count*5, saveadc_data.at24_w_data,strlen((char *)saveadc_data.at24_w_data));//前三个字节是采样总个数
							
							
							saveadc_data.data_count++;
							for(uint8_t i=0;i<2;i++)
							{
								saveadc_data.data_countbuf[i]				  = 0xff&(saveadc_data.data_count>>(i*8));
							}			
							AT24CXX_WriteData(0x0000, saveadc_data.data_countbuf, 2);
						}
						else
						{
							handle.flagstate = STOP;
						}
						statecnt=0;
					}
				}
			break;
			
			case SEND://2 
				HAL_GPIO_WritePin(GPIOA, LED_B_Pin, GPIO_PIN_SET);
				saveadc_data.send_count=((saveadc_data.data_count*5+3)/128)+1;
				for(uint16_t j=0;j<saveadc_data.send_count;j++)//每次读出512个字节发送出去
				{
					memset(saveadc_data.at24_r_data,0,128);
					HAL_I2C_Mem_Read_DMA(&hi2c1, R_IIC_ADDR_AT24CXX, 128*j+3, I2C_MEMADD_SIZE_16BIT, saveadc_data.at24_r_data, 128);
					while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
					HAL_UART_Transmit_DMA(&huart1, saveadc_data.at24_r_data, 128);
					HAL_Delay(20);
				}
				handle.flagstate = STOP;
			break;
			
			case CLEAN://3
				saveadc_data.data_count=0;
				//AT24CXX_WriteData(0x0000, cleancount, 2);
				for(uint16_t j=0;j<512;j++)
				{
					AT24CXX_WriteData(128*j, saveadc_data.clean_data, 128);
				}						
				//memset(ADC_BUF,0,ADC_BUFLEN);
				handle.flagstate = SAVE;
				handle.savecount = 0;
				HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
			break;
			
			case STOP://4 	
				HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, LED_B_Pin, GPIO_PIN_RESET);
				if(saveadc_data.data_count<13000)
				{
					if(read_adc.usb < 1.5)
					{
						handle.flagstate = SAVE;
					}
					
				}
			break;
			
			case WRITE://5
//				read_adc.charge = read_adc.adc_value[0]*3.3f/4096;	
//				sprintf((char *)saveadc_data.at24_w_data,"%.2f,",read_adc.charge);
//				AT24CXX_WriteData(0x0000, saveadc_data.at24_w_data,5);
				AT24CXX_WriteData(0x0000, saveadc_data.at24_w_data,8);			
				handle.flagstate = STOP;
			break;
			case READ://6 	
				HAL_I2C_Mem_Read_DMA(&hi2c1, R_IIC_ADDR_AT24CXX, 0x0000, I2C_MEMADD_SIZE_16BIT, saveadc_data.at24_r_data, 64);
				while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
				handle.flagstate = STOP;					
			break;			  
		}		
}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
