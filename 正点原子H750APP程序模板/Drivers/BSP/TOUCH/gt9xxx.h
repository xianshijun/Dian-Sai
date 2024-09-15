/**
 ****************************************************************************************************
 * @file        gt9xxx.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       GT9XXX驱动代码
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

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "delay.h"

/* 引脚定义 */
#define GT9XXX_RST_GPIO_PORT            GPIOI
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_8
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)
#define GT9XXX_INT_GPIO_PORT            GPIOH
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_7
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)

/* IO操作 */
#define GT9XXX_RST(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define GT9XXX_INT                      ((HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* GT9XXX IIC读写地址定义 */
#define GT9XXX_CMD_WR                   0x28    /* 写地址 */
#define GT9XXX_CMD_RD                   0x29    /* 读地址 */

/* GT9XXX部分寄存器地址定义 */
#define GT9XXX_CTRL_REG                 0x8040  /* GT9XXX控制寄存器 */
#define GT9XXX_CFGS_REG                 0x8047  /* GT9XXX配置起始地址寄存器 */
#define GT9XXX_CHECK_REG                0x80FF  /* GT9XXX校验和寄存器 */
#define GT9XXX_PID_REG                  0x8140  /* GT9XXX产品ID寄存器 */
#define GT9XXX_GSTID_REG                0x814E  /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG                  0x8150  /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG                  0x8158  /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG                  0x8160  /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG                  0x8168  /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG                  0x8170  /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG                  0x8178  /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG                  0x8180  /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG                  0x8188  /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG                  0x8190  /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG                 0x8198  /* 第十个触摸点数据地址 */

/* 函数声明 */
uint8_t gt9xxx_init(void);          /* 初始化GT9XXX */
uint8_t gt9xxx_scan(uint8_t mode);  /* 扫描GT9XXX触摸屏 */

#endif
