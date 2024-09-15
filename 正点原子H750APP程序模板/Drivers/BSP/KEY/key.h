/**
 ****************************************************************************************************
 * @file        key.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       ������������
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

#ifndef __KEY_H
#define __KEY_H

#include "delay.h"

/* ���Ŷ��� */
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

/* IO���� */
#define WKUP                    ((HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY0                    ((HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY1                    ((HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define KEY2                    ((HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* ��ֵ���� */
#define NONE_PRES               0   /* û�а������� */
#define WKUP_PRES               1   /* WKUP�������� */
#define KEY0_PRES               2   /* KEY0�������� */
#define KEY1_PRES               3   /* KEY1�������� */
#define KEY2_PRES               4   /* KEY2�������� */

/* �������� */
void key_init(void);            /* ��ʼ������ */
uint8_t key_scan(uint8_t mode); /* ɨ�谴�� */
void Keying_Function(void);
#endif
