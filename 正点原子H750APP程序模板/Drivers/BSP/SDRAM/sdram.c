/**
 ****************************************************************************************************
 * @file        sdram.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       SDRAM��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:����ԭ�� ������ H750������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "sdram.h"
#include "delay.h"
#include "stm32h7xx_hal_sdram.h"
/* SDRAM���ò��� */
#define SDRAM_MODEREG_BURST_LENGTH_1                0x00000000
#define SDRAM_MODEREG_BURST_LENGTH_2                0x00000001
#define SDRAM_MODEREG_BURST_LENGTH_4                0x00000002
#define SDRAM_MODEREG_BURST_LENGTH_8                0x00000004
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL         0x00000000
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED        0x00000008
#define SDRAM_MODEREG_CAS_LATENCY_2                 0x00000020
#define SDRAM_MODEREG_CAS_LATENCY_3                 0x00000030
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD       0x00000000
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED    0x00000000
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE        0x00000200

SDRAM_HandleTypeDef g_sdram_handle = {0};   /* SDRAM��� */

/**
 * @brief   SDRAMʱ���ʼ��
 * @param   ��
 * @retval  ��
 */
static void sdram_initialization_sequence(void)
{
    FMC_SDRAM_CommandTypeDef fmc_sdram_command_struct = {0};
    
    /* ʹ��ʱ�� */
    fmc_sdram_command_struct.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    fmc_sdram_command_struct.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    fmc_sdram_command_struct.AutoRefreshNumber = 1;
    fmc_sdram_command_struct.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&g_sdram_handle, &fmc_sdram_command_struct, 0xFFFF);
    
    /* ���ٵȴ�100us */
    delay_us(500);
    
    /* Ԥ��� */
    fmc_sdram_command_struct.CommandMode = FMC_SDRAM_CMD_PALL;
    fmc_sdram_command_struct.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    fmc_sdram_command_struct.AutoRefreshNumber = 1;
    fmc_sdram_command_struct.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&g_sdram_handle, &fmc_sdram_command_struct, 0xFFFF);
    
    /* ��ˢ�� */
    fmc_sdram_command_struct.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    fmc_sdram_command_struct.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    fmc_sdram_command_struct.AutoRefreshNumber = 8;
    fmc_sdram_command_struct.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&g_sdram_handle, &fmc_sdram_command_struct, 0xFFFF);
    
    /* ����ģʽ�Ĵ��� */
    fmc_sdram_command_struct.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    fmc_sdram_command_struct.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    fmc_sdram_command_struct.AutoRefreshNumber = 1;
    fmc_sdram_command_struct.ModeRegisterDefinition = SDRAM_MODEREG_BURST_LENGTH_1 |
                                                      SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                                                      SDRAM_MODEREG_CAS_LATENCY_2 |
                                                      SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                                      SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    HAL_SDRAM_SendCommand(&g_sdram_handle, &fmc_sdram_command_struct, 0xFFFF);
    
    /* ����ˢ�¼����� */
    HAL_SDRAM_ProgramRefreshRate(&g_sdram_handle, 839);
}

/**
 * @brief   ��ʼ��SDRAM
 * @param   ��
 * @retval  ��
 */
void sdram_init(void)
{
    FMC_SDRAM_TimingTypeDef fmc_sdram_timing_struct = {0};
    
    /* ����SDRAM������ʱ�� */
    fmc_sdram_timing_struct.LoadToActiveDelay = 2;
    fmc_sdram_timing_struct.ExitSelfRefreshDelay = 8;
    fmc_sdram_timing_struct.SelfRefreshTime = 7;
    fmc_sdram_timing_struct.RowCycleDelay = 7;
    fmc_sdram_timing_struct.WriteRecoveryTime = 2;
    fmc_sdram_timing_struct.RPDelay = 2;
    fmc_sdram_timing_struct.RCDDelay = 2;
    
    /* ��ʼ��SDRAM */
    g_sdram_handle.Instance = FMC_SDRAM_DEVICE;
    g_sdram_handle.Init.SDBank = FMC_SDRAM_BANK1;
    g_sdram_handle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    g_sdram_handle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
    g_sdram_handle.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_32;
    g_sdram_handle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    g_sdram_handle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
    g_sdram_handle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    g_sdram_handle.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
    g_sdram_handle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    g_sdram_handle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
    HAL_SDRAM_Init(&g_sdram_handle, &fmc_sdram_timing_struct);
    
    /* SDRAMʱ���ʼ�� */
    sdram_initialization_sequence();
		
	  printf("%s\r\n",__FUNCTION__);
}

///**
// * @brief   HAL��SDRAM��ʼ��MSP����
// * @param   ��
// * @retval  ��
// */
//void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
//{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    if (hsdram->Instance == FMC_SDRAM_DEVICE)
//    {
//        /* ����ʱ�� */
//        rcc_periph_clk_init_struct.PeriphClockSelection |= RCC_PERIPHCLK_FMC;
//        rcc_periph_clk_init_struct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
//        HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct);
//        
//        __HAL_RCC_FMC_CLK_ENABLE();
//        __HAL_RCC_GPIOC_CLK_ENABLE();
//        __HAL_RCC_GPIOD_CLK_ENABLE();
//        __HAL_RCC_GPIOE_CLK_ENABLE();
//        __HAL_RCC_GPIOF_CLK_ENABLE();
//        __HAL_RCC_GPIOG_CLK_ENABLE();
//        __HAL_RCC_GPIOH_CLK_ENABLE();
//        __HAL_RCC_GPIOI_CLK_ENABLE();
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_SDNWE */
//                               GPIO_PIN_2 |     /* FMC_SDNE0 */
//                               GPIO_PIN_3;      /* FMC_SDCKE0 */
//        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
//        gpio_init_struct.Pull = GPIO_PULLUP;
//        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//        gpio_init_struct.Alternate = GPIO_AF12_FMC;
//        HAL_GPIO_Init(GPIOC, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_D2 */
//                               GPIO_PIN_1 |     /* FMC_D3 */
//                               GPIO_PIN_8 |     /* FMC_D13 */
//                               GPIO_PIN_9 |     /* FMC_D14 */
//                               GPIO_PIN_10 |    /* FMC_D15 */
//                               GPIO_PIN_14 |    /* FMC_D0 */
//                               GPIO_PIN_15;     /* FMC_D1 */
//        HAL_GPIO_Init(GPIOD, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_NBL0 */
//                               GPIO_PIN_1 |     /* FMC_NBL1 */
//                               GPIO_PIN_7 |     /* FMC_D4 */
//                               GPIO_PIN_8 |     /* FMC_D5 */
//                               GPIO_PIN_9 |     /* FMC_D6 */
//                               GPIO_PIN_10 |    /* FMC_D7 */
//                               GPIO_PIN_11 |    /* FMC_D8 */
//                               GPIO_PIN_12 |    /* FMC_D9 */
//                               GPIO_PIN_13 |    /* FMC_D10 */
//                               GPIO_PIN_14 |    /* FMC_D11 */
//                               GPIO_PIN_15;     /* FMC_D12 */
//        HAL_GPIO_Init(GPIOE, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_A0 */
//                               GPIO_PIN_1 |     /* FMC_A1 */
//                               GPIO_PIN_2 |     /* FMC_A2 */
//                               GPIO_PIN_3 |     /* FMC_A3 */
//                               GPIO_PIN_4 |     /* FMC_A4 */
//                               GPIO_PIN_5 |     /* FMC_A5 */
//                               GPIO_PIN_11 |    /* FMC_SDNRAS */
//                               GPIO_PIN_12 |    /* FMC_A6 */
//                               GPIO_PIN_13 |    /* FMC_A7 */
//                               GPIO_PIN_14 |    /* FMC_A8 */
//                               GPIO_PIN_15;     /* FMC_A9 */
//        HAL_GPIO_Init(GPIOF, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_A10 */
//                               GPIO_PIN_1 |     /* FMC_A11 */
//                               GPIO_PIN_2 |     /* FMC_A12 */
//                               GPIO_PIN_4 |     /* FMC_BA0 */
//                               GPIO_PIN_5 |     /* FMC_BA1 */
//                               GPIO_PIN_8 |     /* FMC_SDCLK */
//                               GPIO_PIN_15;     /* FMC_SDNCAS */
//        HAL_GPIO_Init(GPIOG, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_8 |     /* FMC_D16 */
//                               GPIO_PIN_9 |     /* FMC_D17 */
//                               GPIO_PIN_10 |    /* FMC_D18 */
//                               GPIO_PIN_11 |    /* FMC_D19 */
//                               GPIO_PIN_12 |    /* FMC_D20 */
//                               GPIO_PIN_13 |    /* FMC_D21 */
//                               GPIO_PIN_14 |    /* FMC_D22 */
//                               GPIO_PIN_15;     /* FMC_D23 */
//        HAL_GPIO_Init(GPIOH, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* FMC_D24 */
//                               GPIO_PIN_1 |     /* FMC_D25 */
//                               GPIO_PIN_2 |     /* FMC_D26 */
//                               GPIO_PIN_3 |     /* FMC_D27 */
//                               GPIO_PIN_4 |     /* FMC_NBL2 */
//                               GPIO_PIN_5 |     /* FMC_NBL3 */
//                               GPIO_PIN_6 |     /* FMC_D28 */
//                               GPIO_PIN_7 |     /* FMC_D29 */
//                               GPIO_PIN_9 |     /* FMC_D30 */
//                               GPIO_PIN_10;     /* FMC_D31 */
//        HAL_GPIO_Init(GPIOI, &gpio_init_struct);
//    }
//}
