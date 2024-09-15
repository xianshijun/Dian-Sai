/**
 ****************************************************************************************************
 * @file        ltdc.h
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

#ifndef __LTDC_H
#define __LTDC_H
#include "delay.h"

/* LTDC LCD部分引脚定义
 * LTDC LCD数据引脚直接在函数ltdc_init()中定义
 */
#define LTDC_BL_GPIO_PORT               GPIOB
#define LTDC_BL_GPIO_PIN                GPIO_PIN_5
#define LTDC_BL_GPIO_CLK_ENABLE()       do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

#define LTDC_DE_GPIO_PORT               GPIOK
#define LTDC_DE_GPIO_PIN                GPIO_PIN_7
#define LTDC_DE_GPIO_AF                 GPIO_AF14_LTDC
#define LTDC_DE_GPIO_CLK_ENABLE()       do { __HAL_RCC_GPIOK_CLK_ENABLE(); } while (0)

#define LTDC_VSYNC_GPIO_PORT            GPIOI
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_13
#define LTDC_VSYNC_GPIO_AF              GPIO_AF14_LTDC
#define LTDC_VSYNC_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)

#define LTDC_HSYNC_GPIO_PORT            GPIOI
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_12
#define LTDC_HSYNC_GPIO_AF              GPIO_AF14_LTDC
#define LTDC_HSYNC_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)

#define LTDC_CLK_GPIO_PORT              GPIOI
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_14
#define LTDC_CLK_GPIO_AF                GPIO_AF14_LTDC
#define LTDC_CLK_GPIO_CLK_ENABLE()      do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)

/* LTDC LCD引脚操作 */
#define LTDC_BL(x)                      do { (x) ?                                                                  \
                                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET):   \
                                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET); \
                                        } while (0)

/* LTDC颜色数据格式定义 */
#define LTDC_PIXFORMAT                  LTDC_PIXEL_FORMAT_RGB565

/* LTDC显存首地址定义 */
#define LTDC_FRAME_BUF_ADDR             SDRAM_BANK_ADDR

/* LTDC LCD重要参数集 */
typedef struct
{
    uint32_t pwidth;        /* LCD面板的宽度 */
    uint32_t pheight;       /* LCD面板的高度 */
    uint16_t hsw;           /* 水平同步宽度 */
    uint16_t vsw;           /* 垂直同步宽度 */
    uint16_t hbp;           /* 水平后廊 */
    uint16_t vbp;           /* 垂直后廊 */
    uint16_t hfp;           /* 水平前廊 */
    uint16_t vfp;           /* 垂直前廊 */
    uint8_t activelayer;    /* LTDC活动层编号 */
    uint8_t dir;            /* 横竖屏标志位，0：竖屏；1：横屏 */
    uint16_t width;         /* LCD宽度 */
    uint16_t height;        /* LCD高度 */
    uint8_t pixsize;        /* 每个像素所占字节数 */
} _ltdc_dev;

/* LTDC LCD参数导出 */
extern _ltdc_dev lcdltdc;

/* 函数声明 */
void ltdc_init(void);                                                                                                                                                   /* 初始化LTDC */
uint16_t ltdc_panelid_read(void);                                                                                                                                       /* 读取LTDC LCD ID */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                                                               /* 配置LTDC层窗口 */
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor);   /* 配置LTDC层参数 */
void ltdc_select_layer(uint8_t layerx);                                                                                                                                 /* 选择LTDC活动层 */
void ltdc_switch(uint8_t sw);                                                                                                                                           /* 开关LTDC */
void ltdc_layer_switch(uint8_t layerx, uint8_t sw);                                                                                                                     /* 开关LTDC层 */
void ltdc_display_dir(uint8_t dir);                                                                                                                                     /* 设置LTDC显示方向 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color);                                                                                                           /* LTDC画点 */
uint32_t ltdc_read_point(uint16_t x, uint16_t y);                                                                                                                       /* 读取个某点的颜色值 */
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);                                                                                     /* LTDC在指定区域内填充单个颜色 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);                                                                              /* LTDC在指定区域内填充指定颜色块 */
void ltdc_clear(uint32_t color);                                                                                                                                        /* LTDC清屏 */

#endif
