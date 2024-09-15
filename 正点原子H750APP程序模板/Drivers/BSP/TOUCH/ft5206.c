/**
 ****************************************************************************************************
 * @file        ft5206.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       FT5206��������
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

#include "ft5206.h"
#include "delay.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"

/**
 * @brief   ��FT5206����һ������
 * @param   reg: ��ʼ�Ĵ�����ַ
 * @param   buf: ���ݱ�����ʼ��ַ
 * @param   len: ���������ݵĳ���
 * @retval  ��
 */
static void ft5206_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);                        /* ����д���� */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* ���ͼĴ�����8λ��ַ */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_RD);                        /* ���Ͷ����� */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* ѭ���������� */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief   ��FT5206д��һ������
 * @param   reg: ��ʼ�Ĵ�����ַ
 * @param   buf: ��д�����ݵ���ʼ��ַ
 * @param   len: ��д�����ݵĳ���
 * @retval  д����
 * @arg     0: �ɹ�
 * @arg     1: ʧ��
 */
static uint8_t ft5206_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);    /* ����д���� */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);       /* ���ͼĴ�����8λ��ַ */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)           /* ѭ��д������ */
    {
        ct_iic_send_byte(buf[i]);
        ret = ct_iic_wait_ack();
        if (ret)
        {
            break;
        }
    }
    ct_iic_stop();
    
    return ret;
}

/**
 * @brief   ��ʼ��FT5206
 * @param   ��
 * @retval  ��ʼ�����
 *          0: �ɹ�
 *          1: ʧ��
 */
uint8_t ft5206_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp[2];
    
    /* ʱ��ʹ�� */
    FT5206_RST_GPIO_CLK_ENABLE();
    FT5206_INT_GPIO_CLK_ENABLE();
    
    /* ����RST���� */
    gpio_init_struct.Pin = FT5206_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_RST_GPIO_PORT, &gpio_init_struct);
    
    /* ����INT���� */
    gpio_init_struct.Pin = FT5206_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_INT_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��������IIC */
    ct_iic_init();
    
    /* ��λFT5206 */
    FT5206_RST(0);
    delay_ms(20);
    FT5206_RST(1);
    delay_ms(50);
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_DEVIDE_MODE, temp, 1);             /* ���ý�����������ģʽ */
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_ID_G_MODE, temp, 1);               /* ��ѯģʽ */
    
    temp[0] = 22;
    ft5206_wr_reg(FT5206_ID_G_THGROUP, temp, 1);            /* ������Чֵ��ԽСԽ���� */
    
    temp[0] = 12;
    ft5206_wr_reg(FT5206_ID_G_PERIODACTIVE, temp, 1);       /* �������ڣ�����С��12�����14 */
    
    ft5206_rd_reg(FT5206_ID_G_LIB_VERSION, temp, 2);        /* ��ȡ�汾�� */
    if (((temp[0] == 0x30) && (temp[1] == 0x03)) ||         /* 0x3003 */
        ((temp[0] == 0x00) && (temp[1] == 0x01)) ||         /* 0x0001 */
        ((temp[0] == 0x00) && (temp[1] == 0x02)) ||         /* 0x0002 */
        ((temp[0] == 0x00) && (temp[1] == 0x00)))           /* CST340 */
    {
        return 0;
    }
    
    return 1;
}

/* FT5206��ȡ��������Ĵ��� */
static const uint16_t FT5206_TPX_TBL[5] = {
    FT5206_TP1_REG,
    FT5206_TP2_REG,
    FT5206_TP3_REG,
    FT5206_TP4_REG,
    FT5206_TP5_REG,
};

/**
 * @brief   ɨ��FT5206��������
 * @note    ���ò�ѯ��ʽ
 * @param   mode: ������δʹ�ã����ڼ��ݵ�����
 * @retval  ��ǰ����״̬
 * @arg     0: �޴���
 * @arg     1: �д���
 */
uint8_t ft5206_scan(uint8_t mode)
{
    static uint8_t t = 0;
    uint16_t temp;
    uint8_t i;
    uint8_t buf[4];
    uint8_t res = 0;
    
    t++;
    if (((t % 10) == 0) || (t < 10))
    {
        ft5206_rd_reg(FT5206_REG_NUM_FINGER, &mode, 1);                                             /* ��ȡ�������״̬ */
        if ((mode & 0x0F) && ((mode & 0x0F) < 6))
        {
            temp = 0xFFFF << (mode & 0x0F);                                                         /* ����ĸ���ת��Ϊ1��λ����ƥ��tp_dev.sta���� */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            delay_ms(4);
            
            for (i=0; i<5; i++)
            {
                if (tp_dev.sta & (1 << i))                                                          /* ��������Ч */
                {
                    ft5206_rd_reg(FT5206_TPX_TBL[i], buf, 4);                                       /* ��ȡXY����ֵ */
                    
                    if (tp_dev.touchtype & 0x01)                                                    /* ���� */
                    {
                        tp_dev.y[i] = ((uint16_t)(buf[0] & 0x0F) << 8) + buf[1];
                        tp_dev.x[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    else                                                                            /* ���� */
                    {
                        tp_dev.x[i] = lcddev.width - (((uint16_t)(buf[0] & 0x0F) << 8) + buf[1]);
                        tp_dev.y[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    
                    if ((buf[0] & 0xF0) != 0x80)
                    {
                        tp_dev.x[i] = tp_dev.y[i] = 0;                                              /* ������contact�¼�������Ϊ��Ч */
                    }
                }
            }
            res = 1;
            
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0)                                               /* ���������ݶ���0������Դ˴����� */
            {
                mode = 0;
            }
            
            t = 0;                                                                                  /* ����һ�Σ���������������10�Σ��Ӷ���������� */
        }
    }
    
    if ((mode & 0x1F) == 0)                                                                         /* �޴����㰴�� */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                                              /* ֮ǰ�Ǳ����µ� */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                                            /* ��ǰ����ɿ� */
        }
        else                                                                                        /* ֮ǰ��û�б����� */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                                   /* �������Ч��� */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                                     /* ���´�10��ʼ���� */
    }
    
    return res;
}
