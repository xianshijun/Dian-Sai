/**
 ****************************************************************************************************
 * @file        ltdc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       LTDC��������
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

#include "myltdc.h"
#include "lcd.h"
#include "sdram.h"
#include "stm32h7xx_hal_ltdc.h"
#include "stm32h7xx_hal_dma2d.h"
LTDC_HandleTypeDef g_ltdc_handle = {0};     /* LTDC��� */
DMA2D_HandleTypeDef g_dma2d_handle = {0};   /* DMA2D��� */

/* LTDC LCD��Ҫ���� */
_ltdc_dev lcdltdc;

/* ����LTDC֡������ */
#if ((LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888) || (LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888))
#if !(__ARMCC_VERSION >= 6010050)
    uint32_t g_ltdc_lcd_framebuf[1280][800] __attribute__((at(LTDC_FRAME_BUF_ADDR)));
#else
    uint32_t g_ltdc_lcd_framebuf[1280][800] __attribute__((section(".bss.ARM.__at_0xC0000000")));
#endif
    uint32_t *g_ltdc_framebuf[2];
#else
#if !(__ARMCC_VERSION >= 6010050)
    uint16_t g_ltdc_lcd_framebuf[1280][800] __attribute__((at(LTDC_FRAME_BUF_ADDR)));
#else
    uint16_t g_ltdc_lcd_framebuf[1280][800] __attribute__((section(".bss.ARM.__at_0xC0000000")));
#endif
    uint16_t *g_ltdc_framebuf[2];
#endif

/**
 * @brief   LTDCʱ������
 * @param   pll3n: PLL3��Ƶϵ��
 * @param   pll3m: PLL3Ԥ��Ƶϵ��
 * @param   pll3r: PLL3 R�����Ƶϵ��
 * @retval  ���ý��
 * @arg     0: ���óɹ�
 * @arg     1: ����ʧ��
 */
static uint8_t ltdc_clk_set(uint32_t pll3n, uint32_t pll3m, uint32_t pll3r)
{
    rcc_periph_clk_init_struct.PeriphClockSelection |= RCC_PERIPHCLK_LTDC;
    rcc_periph_clk_init_struct.PLL3.PLL3M = pll3m;
    rcc_periph_clk_init_struct.PLL3.PLL3N = pll3n;
    rcc_periph_clk_init_struct.PLL3.PLL3R = pll3r;
    if (HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   ��ʼ��LTDC
 * @param   ��
 * @retval  ��
 */
void ltdc_init(void)
{
    uint16_t id;
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    id = ltdc_panelid_read();
    if (id == 0x4342)               /* ATK-MD0430R-480272 */
    {
        lcdltdc.pwidth = 480;       /* LCD���Ŀ�� */
        lcdltdc.pheight = 272;      /* LCD���ĸ߶� */
        lcdltdc.hsw = 1;            /* ˮƽͬ����� */
        lcdltdc.vsw = 1;            /* ��ֱͬ����� */
        lcdltdc.hbp = 40;           /* ˮƽ���� */
        lcdltdc.vbp = 8;            /* ��ֱ���� */
        lcdltdc.hfp = 5;            /* ˮƽǰ�� */
        lcdltdc.vfp = 8;            /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 33);  /* LTDC_CLK = 9MHz */
    }
    else if (id == 0x7084)          /* ATK-MD0700R-800480 */
    {
        lcdltdc.pwidth = 800;       /* LCD���Ŀ�� */
        lcdltdc.pheight = 480;      /* LCD���ĸ߶� */
        lcdltdc.hsw = 1;            /* ˮƽͬ����� */
        lcdltdc.vsw = 1;            /* ��ֱͬ����� */
        lcdltdc.hbp = 46;           /* ˮƽ���� */
        lcdltdc.vbp = 23;           /* ��ֱ���� */
        lcdltdc.hfp = 210;          /* ˮƽǰ�� */
        lcdltdc.vfp = 22;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 9);   /* LTDC_CLK = 33MHz */
    }
    else if (id == 0x7016)          /* ATK-MD0700R-1024600 */
    {
        lcdltdc.pwidth = 1024;      /* LCD���Ŀ�� */
        lcdltdc.pheight = 600;      /* LCD���ĸ߶� */
        lcdltdc.hsw = 20;           /* ˮƽͬ����� */
        lcdltdc.vsw = 3;            /* ��ֱͬ����� */
        lcdltdc.hbp = 140;          /* ˮƽ���� */
        lcdltdc.vbp = 20;           /* ��ֱ���� */
        lcdltdc.hfp = 160;          /* ˮƽǰ�� */
        lcdltdc.vfp = 12;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 6);   /* LTDC_CLK = 40MHz */
    }
    else if (id == 0x7018)          /* ATK-MD0700R-1280800 */
    {
        lcdltdc.pwidth = 1280;      /* LCD���Ŀ�� */
        lcdltdc.pheight = 800;      /* LCD���ĸ߶� */
        /* ������������ */
    }
    else if (id == 0x4384)          /* ATK-MD0430R-800480 */
    {
        lcdltdc.pwidth = 800;       /* LCD���Ŀ�� */
        lcdltdc.pheight = 480;      /* LCD���ĸ߶� */
        lcdltdc.hsw = 88;           /* ˮƽͬ����� */
        lcdltdc.vsw = 40;           /* ��ֱͬ����� */
        lcdltdc.hbp = 48;           /* ˮƽ���� */
        lcdltdc.vbp = 32;           /* ��ֱ���� */
        lcdltdc.hfp = 13;           /* ˮƽǰ�� */
        lcdltdc.vfp = 3;            /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 9);   /* LTDC_CLK = 33MHz */
    }
    else if (id == 0x1018)          /* ATK-MD1018R-1280800 */
    {
        lcdltdc.pwidth = 1280;      /* LCD���Ŀ�� */
        lcdltdc.pheight = 800;      /* LCD���ĸ߶� */
        lcdltdc.hsw = 140;          /* ˮƽͬ����� */
        lcdltdc.vsw = 10;           /* ��ֱͬ����� */
        lcdltdc.hbp = 10;           /* ˮƽ���� */
        lcdltdc.vbp = 10;           /* ��ֱ���� */
        lcdltdc.hfp = 10;           /* ˮƽǰ�� */
        lcdltdc.vfp = 3;            /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 6);   /* LTDC_CLK = 45MHz */
    }
    
    lcddev.width = lcdltdc.pwidth;
    lcddev.height = lcdltdc.pheight;
    
#if ((LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888) || (LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888))
    g_ltdc_framebuf[0] = (uint32_t *)g_ltdc_lcd_framebuf;
    lcdltdc.pixsize = 4;
#else
    g_ltdc_framebuf[0] = (uint16_t *)g_ltdc_lcd_framebuf;
    lcdltdc.pixsize = 2;
#endif
    
    /* ʹ��ʱ�� */
    LTDC_BL_GPIO_CLK_ENABLE();
    
    /* ����LTDC LCD BL���� */
    gpio_init_struct.Pin = LTDC_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��LTDC */
    g_ltdc_handle.Instance = LTDC;
    g_ltdc_handle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    g_ltdc_handle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    g_ltdc_handle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    if (id == 0x1018)
    {
        g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;
    }
    else
    {
        g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    }
    g_ltdc_handle.Init.HorizontalSync = lcdltdc.hsw - 1;
    g_ltdc_handle.Init.VerticalSync = lcdltdc.vsw - 1;
    g_ltdc_handle.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1;
    g_ltdc_handle.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1;
    g_ltdc_handle.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1;
    g_ltdc_handle.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1;
    g_ltdc_handle.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1;
    g_ltdc_handle.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1;
    g_ltdc_handle.Init.Backcolor.Red = 0;
    g_ltdc_handle.Init.Backcolor.Green = 0;
    g_ltdc_handle.Init.Backcolor.Blue = 0;
    HAL_LTDC_Init(&g_ltdc_handle);
    
    /* ����LTDC�� */
    ltdc_layer_parameter_config(0, (uint32_t)g_ltdc_framebuf[0], LTDC_PIXFORMAT, 255, 0, 6, 7, 0);
    ltdc_layer_window_config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);
    
    ltdc_select_layer(0);
    LTDC_BL(1);
    ltdc_clear(0xFFFFFFFF);
}

///**
// * @brief   HAL��LTDC��ʼ��MSP����
// * @param   ��
// * @retval  ��
// */
//void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
//{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    if (hltdc->Instance == LTDC)
//    {
//        /* ʹ��ʱ�� */
//        __HAL_RCC_LTDC_CLK_ENABLE();
//        __HAL_RCC_DMA2D_CLK_ENABLE();
//        LTDC_BL_GPIO_CLK_ENABLE();
//        LTDC_DE_GPIO_CLK_ENABLE();
//        LTDC_VSYNC_GPIO_CLK_ENABLE();
//        LTDC_HSYNC_GPIO_CLK_ENABLE();
//        LTDC_CLK_GPIO_CLK_ENABLE();
//        __HAL_RCC_GPIOI_CLK_ENABLE();
//        __HAL_RCC_GPIOJ_CLK_ENABLE();
//        __HAL_RCC_GPIOK_CLK_ENABLE();
//        
//        /* ����LTDC DE���� */
//        gpio_init_struct.Pin = LTDC_DE_GPIO_PIN;
//        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
//        gpio_init_struct.Pull = GPIO_NOPULL;
//        gpio_init_struct.Speed = GPIO_SPEED_HIGH;
//        gpio_init_struct.Alternate = LTDC_DE_GPIO_AF;
//        HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &gpio_init_struct);
//        
//        /* ����LTDC VSYNC���� */
//        gpio_init_struct.Pin = LTDC_VSYNC_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_VSYNC_GPIO_AF;
//        HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &gpio_init_struct);
//        
//        /* ����LTDC HSYNC���� */
//        gpio_init_struct.Pin = LTDC_HSYNC_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_HSYNC_GPIO_AF;
//        HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &gpio_init_struct);
//        
//        /* ����LTDC CLK���� */
//        gpio_init_struct.Pin = LTDC_CLK_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_CLK_GPIO_AF;
//        HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &gpio_init_struct);
//        
//        /* ����LTDC�������� */
//        gpio_init_struct.Pin = GPIO_PIN_15;     /* LTDC_R0 */
//        gpio_init_struct.Alternate = GPIO_AF14_LTDC;
//        HAL_GPIO_Init(GPIOI, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* LTDC_R1 */
//                               GPIO_PIN_1 |     /* LTDC_R2 */
//                               GPIO_PIN_2 |     /* LTDC_R3 */
//                               GPIO_PIN_3 |     /* LTDC_R4 */
//                               GPIO_PIN_4 |     /* LTDC_R5 */
//                               GPIO_PIN_5 |     /* LTDC_R6 */
//                               GPIO_PIN_6 |     /* LTDC_R7 */
//                               GPIO_PIN_7 |     /* LTDC_G0 */
//                               GPIO_PIN_8 |     /* LTDC_G1 */
//                               GPIO_PIN_9 |     /* LTDC_G2 */
//                               GPIO_PIN_10 |    /* LTDC_G3 */
//                               GPIO_PIN_11 |    /* LTDC_G4 */
//                               GPIO_PIN_12 |    /* LTDC_B0 */
//                               GPIO_PIN_13 |    /* LTDC_B1 */
//                               GPIO_PIN_14 |    /* LTDC_B2 */
//                               GPIO_PIN_15;     /* LTDC_B3 */
//        HAL_GPIO_Init(GPIOJ, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_0 |     /* LTDC_G5 */
//                               GPIO_PIN_1 |     /* LTDC_G6 */
//                               GPIO_PIN_2 |     /* LTDC_G7 */
//                               GPIO_PIN_3 |     /* LTDC_B4 */
//                               GPIO_PIN_4 |     /* LTDC_B5 */
//                               GPIO_PIN_5 |     /* LTDC_B6 */
//                               GPIO_PIN_6;      /* LTDC_B7 */
//        HAL_GPIO_Init(GPIOK, &gpio_init_struct);
//    }
//}

/**
 * @brief   ��ȡLTDC LCD ID
 * @param   ��
 * @retval  LTDC LCD ID
 */
uint16_t ltdc_panelid_read(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t id;
    
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    
    gpio_init_struct.Pin = GPIO_PIN_6;          /* LTDC_R7 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOJ, &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_2 |         /* LTDC_G7 */
                           GPIO_PIN_6;          /* LTDC_B7 */
    HAL_GPIO_Init(GPIOK, &gpio_init_struct);
    
    id = ((HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0 : 1);
    id |= ((HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_2) == GPIO_PIN_RESET) ? 0 : 1) << 1;
    id |= ((HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0 : 1) << 2;
    
    switch (id)
    {
        case 0:
        {
            return 0x4342;                      /* ATK-MD0430R-480272 */
        }
        case 1:
        {
            return 0x7084;                      /* ATK-MD0700R-800480 */
        }
        case 2:
        {
            return 0x7016;                      /* ATK-MD0700R-1024600 */
        }
        case 3:
        {
            return 0x7018;                      /* ATK-MD0700R-1280800 */
        }
        case 4:
        {
            return 0x4384;                      /* ATK-MD0430R-800480 */
        }
        case 5:
        {
            return 0x1018;                      /* ATK-MD1018R-1280800 */
        }
        default:
        {
            return 0;
        }
    }
}

/**
 * @brief   ����LTDC�㴰��
 * @param   layerx: LTDC����
 * @arg     0: ��1
 * @arg     1: ��2
 * @param   sx: ��ʼX����
 * @param   sy: ��ʼY����
 * @param   width: ���
 * @param   height: �߶�
 * @retval  ��
 */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&g_ltdc_handle, sx, sy, layerx);
    HAL_LTDC_SetWindowSize(&g_ltdc_handle, width, height, layerx);
}

/**
 * @brief   ����LTDC�����
 * @param   layerx: LTDC����
 * @arg     0: ��1
 * @arg     1: ��2
 * @param   bufaddr: ���Դ���ʼ��ַ
 * @param   pixformat: ����ɫ���ݸ�ʽ
 * @arg     0: ARGB8888
 * @arg     1: RGB888
 * @arg     2: RGB565
 * @arg     3: ARGB1555
 * @arg     4: ARGB4444
 * @arg     5: L8
 * @arg     6: AL44
 * @arg     7: AL88
 * @param   alpha: ����ɫ͸����
 * @param   alpha0: ��Ĭ����ɫ͸����
 * @param   bfac1: ����ϵ��1
 * @param   bfac2: ����ϵ��2
 * @param   bkcolor: ��Ĭ����ɫ��RGB888��ʽ��
 * @retval  ��
 */
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor)
{
    LTDC_LayerCfgTypeDef ltdc_layer_cfg_struct = {0};
    
    ltdc_layer_cfg_struct.WindowX0 = 0;
    ltdc_layer_cfg_struct.WindowX1 = 0;
    ltdc_layer_cfg_struct.WindowY0 = lcdltdc.pwidth;
    ltdc_layer_cfg_struct.WindowY1 = lcdltdc.pheight;
    ltdc_layer_cfg_struct.PixelFormat = pixformat;
    ltdc_layer_cfg_struct.Alpha = alpha;
    ltdc_layer_cfg_struct.Alpha0 = alpha0;
    ltdc_layer_cfg_struct.BlendingFactor1 = (uint32_t)bfac1 << 8;
    ltdc_layer_cfg_struct.BlendingFactor2 = (uint32_t)bfac2 << 0;
    ltdc_layer_cfg_struct.FBStartAdress = bufaddr;
    ltdc_layer_cfg_struct.ImageWidth = lcdltdc.pwidth;
    ltdc_layer_cfg_struct.ImageHeight = lcdltdc.pheight;
    ltdc_layer_cfg_struct.Backcolor.Red = (uint8_t)(bkcolor & 0X00FF0000) >> 16;
    ltdc_layer_cfg_struct.Backcolor.Green = (uint8_t)(bkcolor & 0X0000FF00) >> 8;
    ltdc_layer_cfg_struct.Backcolor.Blue = (uint8_t)bkcolor & 0X000000FF;
    HAL_LTDC_ConfigLayer(&g_ltdc_handle, &ltdc_layer_cfg_struct, layerx);
}

/**
 * @brief   ѡ��LTDC���
 * @param   layerx: LTDC����
 * @arg     0: ��1
 * @arg     1: ��2
 * @retval  ��
 */
void ltdc_select_layer(uint8_t layerx)
{
    lcdltdc.activelayer = layerx;
}

/**
 * @brief   ����LTDC
 * @param   sw: ����״̬
 * @arg     0: �ر�
 * @arg     1: ����
 * @retval  ��
 */
void ltdc_switch(uint8_t sw)
{
    if (sw != 0)
    {
        __HAL_LTDC_ENABLE(&g_ltdc_handle);
    }
    else
    {
        __HAL_LTDC_DISABLE(&g_ltdc_handle);
    }
}

/**
 * @brief   ����LTDC��
 * @param   layerx: LTDC����
 * @arg     0: ��1
 * @arg     1: ��2
 * @param   sw: ����״̬
 * @arg     0: �ر�
 * @arg     1: ����
 * @retval  ��
 */
void ltdc_layer_switch(uint8_t layerx, uint8_t sw)
{
    if (sw != 0)
    {
        __HAL_LTDC_LAYER_ENABLE(&g_ltdc_handle, layerx);
    }
    else
    {
        __HAL_LTDC_LAYER_DISABLE(&g_ltdc_handle, layerx);
    }
    __HAL_LTDC_RELOAD_CONFIG(&g_ltdc_handle);
}

/**
 * @brief   ����LTDC��ʾ����
 * @param   dir: LTDC��ʾ����
 * @arg     0: ����
 * @arg     1: ����
 * @retval  ��
 */
void ltdc_display_dir(uint8_t dir)
{
    lcdltdc.dir = dir;
    
    if (lcdltdc.dir == 0)
    {
        lcdltdc.width = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }
    else
    {
        lcdltdc.width = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/**
 * @brief   LTDC����
 * @param   x: ���X����
 * @param   y: ���Y����
 * @param   color: �����ɫ
 * @retval  ��
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
#if ((LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888) || (LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888))
    if (lcdltdc.dir == 0)
    {
        *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
    }
    else
    {
        *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
#else
    if (lcdltdc.dir == 0)
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
    }
    else
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
#endif
}

/**
 * @brief   ��ȡ��ĳ�����ɫֵ
 * @param   x: ָ�����X����
 * @param   y: ָ�����Y����
 * @retval  ָ�������ɫ
 */
uint32_t ltdc_read_point(uint16_t x, uint16_t y)
{
#if ((LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888) || (LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888))
    if (lcdltdc.dir == 0)
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
    }
    else
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
#else
    if (lcdltdc.dir == 0)
    {
        return *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
    }
    else
    {
        return *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
#endif
}

/**
 * @brief   LTDC��ָ����������䵥����ɫ
 * @param   sx: ָ���������ʼX����
 * @param   sy: ָ���������ʼY����
 * @param   ex: ָ������Ľ���X����
 * @param   ey: ָ������Ľ���Y����
 * @param   color: Ҫ������ɫ
 * @retval  ��
 */
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint32_t psx;
    uint32_t psy;
    uint32_t pex;
    uint32_t pey;
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    
    if (lcdltdc.dir == 0)
    {
        if (ex >= lcdltdc.pheight)
        {
            ex = lcdltdc.pheight - 1;
        }
        
        if (sx >= lcdltdc.pheight)
        {
            sx = lcdltdc.pheight - 1;
        }
        
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }
    else
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    
    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
    
    /* ����DMA2D */
    DMA2D->CR &= ~(DMA2D_CR_START);
    DMA2D->CR = DMA2D_R2M;
    DMA2D->OPFCCR = LTDC_PIXFORMAT;
    DMA2D->OOR = offline;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);
    DMA2D->OCOLR = color;
    
    /* ����DMA2D */
    DMA2D->CR |= DMA2D_CR_START;
    
    /* �ȴ�DMA2D������� */
    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)
    {
        if (++timeout > 0x1FFFFF)
        {
            break;
        }
    } 
    
    /* ���DMA2D������ɱ�־ */
    DMA2D->IFCR |= DMA2D_FLAG_TC;
}

/**
 * @brief   LTDC��ָ�����������ָ����ɫ��
 * @param   sx: ָ���������ʼX����
 * @param   sy: ָ���������ʼY����
 * @param   ex: ָ������Ľ���X����
 * @param   ey: ָ������Ľ���Y����
 * @param   color: ָ����ɫ������׵�ַ
 * @retval  ��
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t psx;
    uint32_t psy;
    uint32_t pex;
    uint32_t pey;
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;
    
    if (lcdltdc.dir == 0)
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }
    else
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    
    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
    
    /* ����DMA2D */
    DMA2D->CR &= ~(DMA2D_CR_START);
    DMA2D->CR = DMA2D_M2M;
    DMA2D->FGPFCCR = LTDC_PIXFORMAT;
    DMA2D->FGOR = 0;
    DMA2D->OOR = offline;
    DMA2D->FGMAR = (uint32_t)color;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);
    
    /* ����DMA2D */
    DMA2D->CR |= DMA2D_CR_START;
    
    /* �ȴ�DMA2D������� */
    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)
    {
        if (++timeout > 0x1FFFFF)
        {
            break;
        }
    } 
    
    /* ���DMA2D������ɱ�־ */
    DMA2D->IFCR |= DMA2D_FLAG_TC;
}

/**
 * @brief   LTDC����
 * @param   color: ��������ɫ
 * @retval  ��
 */
void ltdc_clear(uint32_t color)
{
    ltdc_fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}
