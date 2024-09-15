/**
 ****************************************************************************************************
 * @file        ltdc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       LTDC驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 北极星 H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "myltdc.h"
#include "lcd.h"
#include "sdram.h"
#include "stm32h7xx_hal_ltdc.h"
#include "stm32h7xx_hal_dma2d.h"
LTDC_HandleTypeDef g_ltdc_handle = {0};     /* LTDC句柄 */
DMA2D_HandleTypeDef g_dma2d_handle = {0};   /* DMA2D句柄 */

/* LTDC LCD重要参数 */
_ltdc_dev lcdltdc;

/* 定义LTDC帧缓冲区 */
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
 * @brief   LTDC时钟设置
 * @param   pll3n: PLL3倍频系数
 * @param   pll3m: PLL3预分频系数
 * @param   pll3r: PLL3 R输出分频系数
 * @retval  设置结果
 * @arg     0: 设置成功
 * @arg     1: 设置失败
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
 * @brief   初始化LTDC
 * @param   无
 * @retval  无
 */
void ltdc_init(void)
{
    uint16_t id;
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    id = ltdc_panelid_read();
    if (id == 0x4342)               /* ATK-MD0430R-480272 */
    {
        lcdltdc.pwidth = 480;       /* LCD面板的宽度 */
        lcdltdc.pheight = 272;      /* LCD面板的高度 */
        lcdltdc.hsw = 1;            /* 水平同步宽度 */
        lcdltdc.vsw = 1;            /* 垂直同步宽度 */
        lcdltdc.hbp = 40;           /* 水平后廊 */
        lcdltdc.vbp = 8;            /* 垂直后廊 */
        lcdltdc.hfp = 5;            /* 水平前廊 */
        lcdltdc.vfp = 8;            /* 垂直前廊 */
        ltdc_clk_set(300, 25, 33);  /* LTDC_CLK = 9MHz */
    }
    else if (id == 0x7084)          /* ATK-MD0700R-800480 */
    {
        lcdltdc.pwidth = 800;       /* LCD面板的宽度 */
        lcdltdc.pheight = 480;      /* LCD面板的高度 */
        lcdltdc.hsw = 1;            /* 水平同步宽度 */
        lcdltdc.vsw = 1;            /* 垂直同步宽度 */
        lcdltdc.hbp = 46;           /* 水平后廊 */
        lcdltdc.vbp = 23;           /* 垂直后廊 */
        lcdltdc.hfp = 210;          /* 水平前廊 */
        lcdltdc.vfp = 22;           /* 垂直前廊 */
        ltdc_clk_set(300, 25, 9);   /* LTDC_CLK = 33MHz */
    }
    else if (id == 0x7016)          /* ATK-MD0700R-1024600 */
    {
        lcdltdc.pwidth = 1024;      /* LCD面板的宽度 */
        lcdltdc.pheight = 600;      /* LCD面板的高度 */
        lcdltdc.hsw = 20;           /* 水平同步宽度 */
        lcdltdc.vsw = 3;            /* 垂直同步宽度 */
        lcdltdc.hbp = 140;          /* 水平后廊 */
        lcdltdc.vbp = 20;           /* 垂直后廊 */
        lcdltdc.hfp = 160;          /* 水平前廊 */
        lcdltdc.vfp = 12;           /* 垂直前廊 */
        ltdc_clk_set(300, 25, 6);   /* LTDC_CLK = 40MHz */
    }
    else if (id == 0x7018)          /* ATK-MD0700R-1280800 */
    {
        lcdltdc.pwidth = 1280;      /* LCD面板的宽度 */
        lcdltdc.pheight = 800;      /* LCD面板的高度 */
        /* 其他参数待定 */
    }
    else if (id == 0x4384)          /* ATK-MD0430R-800480 */
    {
        lcdltdc.pwidth = 800;       /* LCD面板的宽度 */
        lcdltdc.pheight = 480;      /* LCD面板的高度 */
        lcdltdc.hsw = 88;           /* 水平同步宽度 */
        lcdltdc.vsw = 40;           /* 垂直同步宽度 */
        lcdltdc.hbp = 48;           /* 水平后廊 */
        lcdltdc.vbp = 32;           /* 垂直后廊 */
        lcdltdc.hfp = 13;           /* 水平前廊 */
        lcdltdc.vfp = 3;            /* 垂直前廊 */
        ltdc_clk_set(300, 25, 9);   /* LTDC_CLK = 33MHz */
    }
    else if (id == 0x1018)          /* ATK-MD1018R-1280800 */
    {
        lcdltdc.pwidth = 1280;      /* LCD面板的宽度 */
        lcdltdc.pheight = 800;      /* LCD面板的高度 */
        lcdltdc.hsw = 140;          /* 水平同步宽度 */
        lcdltdc.vsw = 10;           /* 垂直同步宽度 */
        lcdltdc.hbp = 10;           /* 水平后廊 */
        lcdltdc.vbp = 10;           /* 垂直后廊 */
        lcdltdc.hfp = 10;           /* 水平前廊 */
        lcdltdc.vfp = 3;            /* 垂直前廊 */
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
    
    /* 使能时钟 */
    LTDC_BL_GPIO_CLK_ENABLE();
    
    /* 配置LTDC LCD BL引脚 */
    gpio_init_struct.Pin = LTDC_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化LTDC */
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
    
    /* 配置LTDC层 */
    ltdc_layer_parameter_config(0, (uint32_t)g_ltdc_framebuf[0], LTDC_PIXFORMAT, 255, 0, 6, 7, 0);
    ltdc_layer_window_config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);
    
    ltdc_select_layer(0);
    LTDC_BL(1);
    ltdc_clear(0xFFFFFFFF);
}

///**
// * @brief   HAL库LTDC初始化MSP函数
// * @param   无
// * @retval  无
// */
//void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
//{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    if (hltdc->Instance == LTDC)
//    {
//        /* 使能时钟 */
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
//        /* 配置LTDC DE引脚 */
//        gpio_init_struct.Pin = LTDC_DE_GPIO_PIN;
//        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
//        gpio_init_struct.Pull = GPIO_NOPULL;
//        gpio_init_struct.Speed = GPIO_SPEED_HIGH;
//        gpio_init_struct.Alternate = LTDC_DE_GPIO_AF;
//        HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &gpio_init_struct);
//        
//        /* 配置LTDC VSYNC引脚 */
//        gpio_init_struct.Pin = LTDC_VSYNC_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_VSYNC_GPIO_AF;
//        HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &gpio_init_struct);
//        
//        /* 配置LTDC HSYNC引脚 */
//        gpio_init_struct.Pin = LTDC_HSYNC_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_HSYNC_GPIO_AF;
//        HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &gpio_init_struct);
//        
//        /* 配置LTDC CLK引脚 */
//        gpio_init_struct.Pin = LTDC_CLK_GPIO_PIN;
//        gpio_init_struct.Alternate = LTDC_CLK_GPIO_AF;
//        HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &gpio_init_struct);
//        
//        /* 配置LTDC数据引脚 */
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
 * @brief   读取LTDC LCD ID
 * @param   无
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
 * @brief   配置LTDC层窗口
 * @param   layerx: LTDC层编号
 * @arg     0: 层1
 * @arg     1: 层2
 * @param   sx: 起始X坐标
 * @param   sy: 起始Y坐标
 * @param   width: 宽度
 * @param   height: 高度
 * @retval  无
 */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&g_ltdc_handle, sx, sy, layerx);
    HAL_LTDC_SetWindowSize(&g_ltdc_handle, width, height, layerx);
}

/**
 * @brief   配置LTDC层参数
 * @param   layerx: LTDC层编号
 * @arg     0: 层1
 * @arg     1: 层2
 * @param   bufaddr: 层显存起始地址
 * @param   pixformat: 层颜色数据格式
 * @arg     0: ARGB8888
 * @arg     1: RGB888
 * @arg     2: RGB565
 * @arg     3: ARGB1555
 * @arg     4: ARGB4444
 * @arg     5: L8
 * @arg     6: AL44
 * @arg     7: AL88
 * @param   alpha: 层颜色透明度
 * @param   alpha0: 层默认颜色透明度
 * @param   bfac1: 层混合系数1
 * @param   bfac2: 层混合系数2
 * @param   bkcolor: 层默认颜色（RGB888格式）
 * @retval  无
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
 * @brief   选择LTDC活动层
 * @param   layerx: LTDC层编号
 * @arg     0: 层1
 * @arg     1: 层2
 * @retval  无
 */
void ltdc_select_layer(uint8_t layerx)
{
    lcdltdc.activelayer = layerx;
}

/**
 * @brief   开关LTDC
 * @param   sw: 开关状态
 * @arg     0: 关闭
 * @arg     1: 开启
 * @retval  无
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
 * @brief   开关LTDC层
 * @param   layerx: LTDC层编号
 * @arg     0: 层1
 * @arg     1: 层2
 * @param   sw: 开关状态
 * @arg     0: 关闭
 * @arg     1: 开启
 * @retval  无
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
 * @brief   设置LTDC显示方向
 * @param   dir: LTDC显示方向
 * @arg     0: 竖屏
 * @arg     1: 横屏
 * @retval  无
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
 * @brief   LTDC画点
 * @param   x: 点的X坐标
 * @param   y: 点的Y坐标
 * @param   color: 点的颜色
 * @retval  无
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
 * @brief   读取个某点的颜色值
 * @param   x: 指定点的X坐标
 * @param   y: 指定点的Y坐标
 * @retval  指定点的颜色
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
 * @brief   LTDC在指定区域内填充单个颜色
 * @param   sx: 指定区域的起始X坐标
 * @param   sy: 指定区域的起始Y坐标
 * @param   ex: 指定区域的结束X坐标
 * @param   ey: 指定区域的结束Y坐标
 * @param   color: 要填充的颜色
 * @retval  无
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
    
    /* 配置DMA2D */
    DMA2D->CR &= ~(DMA2D_CR_START);
    DMA2D->CR = DMA2D_R2M;
    DMA2D->OPFCCR = LTDC_PIXFORMAT;
    DMA2D->OOR = offline;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);
    DMA2D->OCOLR = color;
    
    /* 启动DMA2D */
    DMA2D->CR |= DMA2D_CR_START;
    
    /* 等待DMA2D传输结束 */
    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)
    {
        if (++timeout > 0x1FFFFF)
        {
            break;
        }
    } 
    
    /* 清除DMA2D传输完成标志 */
    DMA2D->IFCR |= DMA2D_FLAG_TC;
}

/**
 * @brief   LTDC在指定区域内填充指定颜色块
 * @param   sx: 指定区域的起始X坐标
 * @param   sy: 指定区域的起始Y坐标
 * @param   ex: 指定区域的结束X坐标
 * @param   ey: 指定区域的结束Y坐标
 * @param   color: 指定颜色数组的首地址
 * @retval  无
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
    
    /* 配置DMA2D */
    DMA2D->CR &= ~(DMA2D_CR_START);
    DMA2D->CR = DMA2D_M2M;
    DMA2D->FGPFCCR = LTDC_PIXFORMAT;
    DMA2D->FGOR = 0;
    DMA2D->OOR = offline;
    DMA2D->FGMAR = (uint32_t)color;
    DMA2D->OMAR = addr;
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);
    
    /* 启动DMA2D */
    DMA2D->CR |= DMA2D_CR_START;
    
    /* 等待DMA2D传输结束 */
    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)
    {
        if (++timeout > 0x1FFFFF)
        {
            break;
        }
    } 
    
    /* 清除DMA2D传输完成标志 */
    DMA2D->IFCR |= DMA2D_FLAG_TC;
}

/**
 * @brief   LTDC清屏
 * @param   color: 清屏的颜色
 * @retval  无
 */
void ltdc_clear(uint32_t color)
{
    ltdc_fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}
