/**
 ****************************************************************************************************
 * @file        sdram.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       SDRAM驱动代码
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

#ifndef __SDRAM_H
#define __SDRAM_H

#include "delay.h"

/* SDRAM块地址定义 */
#define SDRAM_BANK_ADDR (0xC0000000UL)

/* 函数声明 */
void sdram_init(void);

#endif
