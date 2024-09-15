/**
 ****************************************************************************************************
 * @file        ft5206.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       FT5206驱动代码
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

#ifndef __FT5206_H
#define __FT5206_H

#include "delay.h"


/* 引脚定义 */
#define FT5206_RST_GPIO_PORT            GPIOI
#define FT5206_RST_GPIO_PIN             GPIO_PIN_8
#define FT5206_RST_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)
#define FT5206_INT_GPIO_PORT            GPIOH
#define FT5206_INT_GPIO_PIN             GPIO_PIN_7
#define FT5206_INT_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)

/* IO操作 */
#define FT5206_RST(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define FT5206_INT                      ((HAL_GPIO_ReadPin(FT5206_INT_GPIO_PORT, FT5206_INT_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* FT5206 IIC读写地址定义 */
#define FT5206_CMD_WR                   0x70    /* 写地址 */
#define FT5206_CMD_RD                   0x71    /* 读地址 */

/* FT5206部分寄存器地址定义 */
#define FT5206_DEVIDE_MODE              0x00    /* FT5206模式控制寄存器 */
#define FT5206_REG_NUM_FINGER           0x02    /* 触摸状态寄存器 */
#define FT5206_TP1_REG                  0x03    /* 第一个触摸点数据地址 */
#define FT5206_TP2_REG                  0x09    /* 第二个触摸点数据地址 */
#define FT5206_TP3_REG                  0x0F    /* 第三个触摸点数据地址 */
#define FT5206_TP4_REG                  0x15    /* 第四个触摸点数据地址 */
#define FT5206_TP5_REG                  0x1B    /* 第五个触摸点数据地址 */ 
#define FT5206_ID_G_LIB_VERSION         0xA1    /* 版本 */
#define FT5206_ID_G_MODE                0xA4    /* FT5206中断模式控制寄存器 */
#define FT5206_ID_G_THGROUP             0x80    /* 触摸有效值设置寄存器 */
#define FT5206_ID_G_PERIODACTIVE        0x88    /* 激活状态周期设置寄存器 */

/* 函数声明 */
uint8_t ft5206_init(void);          /* 初始化FT5206 */
uint8_t ft5206_scan(uint8_t mode);  /* 扫描FT5206读触摸屏 */

#endif
