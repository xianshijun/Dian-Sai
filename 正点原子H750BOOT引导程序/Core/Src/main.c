/***
	*************************************************************************************************
	*	@file  	main.c
	*	@version V1.0
	*  @date    2022-7-12
	*	@brief   BOOT����
   *************************************************************************************************
>>>>> ����˵����
	*
	*	1.��ʼ��ʱ�Ӻ�QSPI�����ҽ�W25Q64ӳ��Ϊ�ڲ��洢��
	*	2.��ת��W25Q64�����û�����
	*
>>>>> ���ڴ�ӡ˵����
	*
	*	USART1ʹ�õ���PA9/PA10�����ڲ�����115200
	*	
	************************************************************************************************
***/
#include "main.h"
#include "led.h"
#include "usart.h"
#include "qspi_w25q64.h"
#include <string.h>

/********************************************** �������� *******************************************/

typedef  void (*pFunction)(void);

pFunction JumpToApplication;


/********************************************** �������� *******************************************/

void SystemClock_Config(void);		// ʱ�ӳ�ʼ��

/***************************************************************************************************
*	�� �� ��: main
*
*	��������: ������
*	
****************************************************************************************************/

int main(void)
{
	SCB_EnableICache();		// ʹ��ICache
	SCB_EnableDCache();		// ʹ��DCache
	HAL_Init();					// ��ʼ��HAL��
	SystemClock_Config();	// ����ϵͳʱ�ӣ���Ƶ480MHz
	LED_Init();					// ��ʼ��LED����
	USART1_Init();				// USART1��ʼ��	
	
	printf("\r\n***************************************\r\n");
	
	QSPI_W25Qxx_Init();	  				 // ��ʼ��W25Q64
	QSPI_W25Qxx_MemoryMappedMode(); 	 // ����QSPIΪ�ڴ�ӳ��ģʽ

	SCB_DisableICache();		// �ر�ICache
	SCB_DisableDCache();		// �ر�Dcache

	
	SysTick->CTRL = 0;		// �ر�SysTick
	SysTick->LOAD = 0;		// ��������ֵ
	SysTick->VAL = 0;			// �������ֵ

	JumpToApplication = (pFunction) (*(__IO uint32_t*) (W25Qxx_Mem_Addr + 4));	// ������ʼ��ַ
	__set_MSP(*(__IO uint32_t*) W25Qxx_Mem_Addr);	// ��������ջָ��
	printf("��ת��W25Q64�����û�����>>>\r\n\r\n");

	JumpToApplication();			// ִ����ת

	while (1)
	{	

	}
}


/****************************************************************************************************/
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 480000000 (CPU Clock)
  *            HCLK(Hz)                       = 240000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  120MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  120MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  120MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  120MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 192
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;	   	// ����QSPIʱ��
  
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;	// ѡ��HCLK(240MHz)��ΪQSPI�ں�ʱ��
  
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
