/**
 ****************************************************************************************************
 * @file        key.c
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

#include "key.h"
#include "delay.h"
#include "stdio.h"
/**
 * @brief   ��ʼ������
 * @param   ��
 * @retval  ��
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʹ��GPIO�˿�ʱ�� */
    WKUP_GPIO_CLK_ENABLE();
    KEY0_GPIO_CLK_ENABLE();
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
    
    /* ����WKUP�������� */
    gpio_init_struct.Pin = WKUP_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WKUP_GPIO_PORT, &gpio_init_struct);
    
    /* ����KEY0�������� */
    gpio_init_struct.Pin = KEY0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY0_GPIO_PORT, &gpio_init_struct);
    
    /* ����KEY1�������� */
    gpio_init_struct.Pin = KEY1_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY1_GPIO_PORT, &gpio_init_struct);
    
    /* ����KEY2�������� */
    gpio_init_struct.Pin = KEY2_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY2_GPIO_PORT, &gpio_init_struct);
		
		printf("%s\r\n",__FUNCTION__);
}

/**
 * @brief   ɨ�谴��
 * @note    ������Ӧ�������ȼ���WKUP > KEY0 > KEY1 > KEY2
 * @param   mode: ɨ��ģʽ
 * @arg     0: ��֧��������
 * @arg     1: ֧��������
 * @retval  ������ֵ
 * @arg     NONE_PRES: û�а�������
 * @arg     WKUP_PRES: WKUP��������
 * @arg     KEY0_PRES: KEY0��������
 * @arg     KEY1_PRES: KEY1��������
 * @arg     KEY2_PRES: KEY2��������
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_release = 1;
    uint8_t key_value = NONE_PRES;
    
    if (mode != 0)
    {
        key_release = 1;
    }
    
    if ((key_release == 1) && ((WKUP == 1) || (KEY0 == 0) || (KEY1 == 0) || (KEY2 == 0)))
    {
        delay_ms(10);
        key_release = 0;
        
        if (KEY2 == 0)
        {
            key_value = KEY2_PRES;
        }
        
        if (KEY1 == 0)
        {
            key_value = KEY1_PRES;
        }
        
        if (KEY0 == 0)
        {
            key_value = KEY0_PRES;
        }
        
        if (WKUP == 1)
        {
            key_value = WKUP_PRES;
        }
    }
    else if ((WKUP == 0) && (KEY0 == 1) && (KEY1 == 1) && (KEY2 == 1))
    {
        key_release = 1;
    }
    
    return key_value;
}

void Keying_Function(void)
{
	uint8_t KEY=0;
	KEY=key_scan(0);//��������ģʽ�İ���ɨ��
	if(KEY==1){printf("KEY_UP������\r\n");}
	if(KEY==2){printf("KEY_0������\r\n");}
	if(KEY==3){printf("KEY_1������\r\n");}
	if(KEY==4){printf("KEY_2������\r\n");}

}
