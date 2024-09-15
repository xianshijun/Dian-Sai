/**
 ****************************************************************************************************
 * @file        key.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       按键驱动代码
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

#ifndef __KEY_H
#define __KEY_H

#include "delay.h"

/* 引脚定义 */
#define WKUP_GPIO_PORT          GPIOA
#define WKUP_GPIO_PIN           GPIO_PIN_0
#define WKUP_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

#define KEY0_GPIO_PORT          GPIOH
#define KEY0_GPIO_PIN           GPIO_PIN_3
#define KEY0_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)

#define KEY1_GPIO_PORT          GPIOH
#define KEY1_GPIO_PIN           GPIO_PIN_2
#define KEY1_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)
#define KEY2_GPIO_PORT          GPIOC
#define KEY2_GPIO_PIN           GPIO_PIN_13
#define KEY2_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)

/* IO操作 */
#define WKUP                    ((HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY0                    ((HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY1                    ((HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY2                    ((HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* 键值定义 */
#define NONE_PRES               0   /* 没有按键按下 */
#define WKUP_PRES               1   /* WKUP按键按下 */
#define KEY0_PRES               2   /* KEY0按键按下 */
#define KEY1_PRES               3   /* KEY1按键按下 */
#define KEY2_PRES               4   /* KEY2按键按下 */

/* 函数声明 */
void key_init(void);            /* 初始化按键 */
uint8_t key_scan(uint8_t mode); /* 扫描按键 */
void Keying_Function(void);
#endif
