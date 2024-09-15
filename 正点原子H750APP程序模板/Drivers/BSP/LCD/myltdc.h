/**
 ****************************************************************************************************
 * @file        ltdc.h
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

#ifndef __LTDC_H
#define __LTDC_H
#include "delay.h"

/* LTDC LCD�������Ŷ���
 * LTDC LCD��������ֱ���ں���ltdc_init()�ж���
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

/* LTDC LCD���Ų��� */
#define LTDC_BL(x)                      do { (x) ?                                                                  \
                                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET):   \
                                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET); \
                                        } while (0)

/* LTDC��ɫ���ݸ�ʽ���� */
#define LTDC_PIXFORMAT                  LTDC_PIXEL_FORMAT_RGB565

/* LTDC�Դ��׵�ַ���� */
#define LTDC_FRAME_BUF_ADDR             SDRAM_BANK_ADDR

/* LTDC LCD��Ҫ������ */
typedef struct
{
    uint32_t pwidth;        /* LCD���Ŀ�� */
    uint32_t pheight;       /* LCD���ĸ߶� */
    uint16_t hsw;           /* ˮƽͬ����� */
    uint16_t vsw;           /* ��ֱͬ����� */
    uint16_t hbp;           /* ˮƽ���� */
    uint16_t vbp;           /* ��ֱ���� */
    uint16_t hfp;           /* ˮƽǰ�� */
    uint16_t vfp;           /* ��ֱǰ�� */
    uint8_t activelayer;    /* LTDC����� */
    uint8_t dir;            /* ��������־λ��0��������1������ */
    uint16_t width;         /* LCD��� */
    uint16_t height;        /* LCD�߶� */
    uint8_t pixsize;        /* ÿ��������ռ�ֽ��� */
} _ltdc_dev;

/* LTDC LCD�������� */
extern _ltdc_dev lcdltdc;

/* �������� */
void ltdc_init(void);                                                                                                                                                   /* ��ʼ��LTDC */
uint16_t ltdc_panelid_read(void);                                                                                                                                       /* ��ȡLTDC LCD ID */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                                                               /* ����LTDC�㴰�� */
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor);   /* ����LTDC����� */
void ltdc_select_layer(uint8_t layerx);                                                                                                                                 /* ѡ��LTDC��� */
void ltdc_switch(uint8_t sw);                                                                                                                                           /* ����LTDC */
void ltdc_layer_switch(uint8_t layerx, uint8_t sw);                                                                                                                     /* ����LTDC�� */
void ltdc_display_dir(uint8_t dir);                                                                                                                                     /* ����LTDC��ʾ���� */
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color);                                                                                                           /* LTDC���� */
uint32_t ltdc_read_point(uint16_t x, uint16_t y);                                                                                                                       /* ��ȡ��ĳ�����ɫֵ */
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);                                                                                     /* LTDC��ָ����������䵥����ɫ */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);                                                                              /* LTDC��ָ�����������ָ����ɫ�� */
void ltdc_clear(uint32_t color);                                                                                                                                        /* LTDC���� */

#endif
