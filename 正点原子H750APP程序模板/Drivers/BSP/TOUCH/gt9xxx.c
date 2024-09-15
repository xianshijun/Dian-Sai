/**
 ****************************************************************************************************
 * @file        gt9xxx.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       GT9XXX��������
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

#include "gt9xxx.h"
#include "delay.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"
#include <string.h>

/* Ĭ��֧�ִ�����5�㴥��
 * ����GT9271֧��10�㴥��������ֻ֧��5�㴥��
 */
static uint8_t g_gt_tnum = 5;

/**
 * @brief   ��GT9XXX����һ������
 * @param   reg: ��ʼ�Ĵ�����ַ
 * @param   buf: ���ݱ�����ʼ��ַ
 * @param   len: ���������ݵĳ���
 * @retval  ��
 */
static void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);                        /* ����д���� */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);                             /* ���ͼĴ�����8λ��ַ */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* ���ͼĴ�����8λ��ַ */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);                        /* ���Ͷ����� */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* ѭ���������� */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief   ��GT9XXXд��һ������
 * @param   reg: ��ʼ�Ĵ�����ַ
 * @param   buf: ��д�����ݵ���ʼ��ַ
 * @param   len: ��д�����ݵĳ���
 * @retval  д����
 * @arg     0: �ɹ�
 * @arg     1: ʧ��
 */
static uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* ����д���� */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* ���ͼĴ�����8λ��ַ */
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
 * @brief   ��ʼ��GT9XXX
 * @param   ��
 * @retval  ��ʼ�����
 * @arg     0: �ɹ�
 * @arg     1: ʧ��
 */
uint8_t gt9xxx_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp[5];
    
    /* ʱ��ʹ�� */
    GT9XXX_RST_GPIO_CLK_ENABLE();
    GT9XXX_INT_GPIO_CLK_ENABLE();
    
    /* ����RST���� */
    gpio_init_struct.Pin = GT9XXX_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_RST_GPIO_PORT, &gpio_init_struct);
    
    /* ����INT���� */
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��������IIC */
    ct_iic_init();
    
    /* ��λGT9XXX */
    GT9XXX_RST(0);
    delay_ms(10);
    GT9XXX_RST(1);
    delay_ms(10);
    
    /* ����INT���� */
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);
    
    delay_ms(100);
    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);                 /* ��ȡGT9XXX PID */
    temp[4] = 0;

    if ((strcmp((char *)temp, "911") != 0) &&
        (strcmp((char *)temp, "9147") != 0) &&
        (strcmp((char *)temp, "1158") != 0) &&
        (strcmp((char *)temp, "9271") != 0))
    {
        return 1;
    }

    if (strcmp((char *)temp, "9271") == 0)                  /* GT9271֧��10�㴥�� */
    {
        g_gt_tnum = 10;
    }
    
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* �����λGT9XXX */
    delay_ms(10);
    
    temp[0] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* ������λ�����������״̬ */
    
    return 0;
}

/* GT9XXX��ȡ��������Ĵ��� */
static const uint16_t GT9XXX_TPX_TBL[10] = {
    GT9XXX_TP1_REG,
    GT9XXX_TP2_REG,
    GT9XXX_TP3_REG,
    GT9XXX_TP4_REG,
    GT9XXX_TP5_REG,
    GT9XXX_TP6_REG,
    GT9XXX_TP7_REG,
    GT9XXX_TP8_REG,
    GT9XXX_TP9_REG,
    GT9XXX_TP10_REG,
};

/**
 * @brief   ɨ��GT9XXX������
 * @note    ���ò�ѯ��ʽ
 * @param   mode: ������δʹ�ã����ڼ��ݵ�����
 * @retval  ��ǰ����״̬
 * @arg     0: �޴���
 * @arg     1: �д���
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    static uint8_t t = 0;
    uint8_t data;
    uint16_t temp;
    uint16_t tempsta;
    uint8_t i;
    uint8_t buf[4];
    uint8_t res = 0;
    
    t++;
    if (((t % 10) == 0) || (t < 10))
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);                                  /* ��ȡ�������״̬ */
        if ((mode & 0x80) && ((mode & 0x0F) <= g_gt_tnum))
        {
            data = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &data, 1);                              /* ���־ */
        }
        
        if ((mode & 0x0F) && ((mode & 0x0F) <= g_gt_tnum))
        {
            temp = 0xFFFF << (mode & 0x0F);                                         /* ����ĸ���ת��Ϊ1��λ����ƥ��tp_dev.sta���� */
            tempsta = tp_dev.sta;                                                   /* ���浱ǰ��tp_dev.staֵ */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];                                  /* ���津��0�����ݣ����������һ���� */
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];
            for (i=0; i<g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))                                          /* ��������Ч */
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);                       /* ��ȡXY����ֵ */
                    
                    if ((lcddev.id == 0x5510) ||                                    /* 4.3��800*480 MCU�� */
                        (lcddev.id == 0x9806) ||
                        (lcddev.id == 0x7796))
                    {
                        if (tp_dev.touchtype & 0x01)                                /* ���� */
                        {
                            tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        }
                        else                                                        /* ���� */
                        {
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                    }
                    else                                                            /* �����ͺ� */
                    {
                        if (tp_dev.touchtype & 0x01)                                /* ���� */
                        {
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                        else                                                        /* ���� */
                        {
                            tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        }
                    }
                }
            }
            res = 1;
            
            if ((tp_dev.x[0] > lcddev.width) || (tp_dev.y[0] > lcddev.height))      /* �Ƿ����ݣ����곬���ˣ� */
            {
                if ((mode & 0x0F) > 1)                                              /* �������������ݣ����Ƶڶ�����������ݵ���һ������. */
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;                                                          /* ����һ�Σ���������������10�Σ��Ӷ���������� */
                }
                else                                                                /* �Ƿ����ݣ�����Դ˴����ݣ���ԭԭ���ģ� */
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0x80;
                    tp_dev.sta = tempsta;                                           /* �ָ�tp_dev.sta */
                }
            }
            else
            {
                t = 0;                                                              /* ����һ�Σ���������������10�Σ��Ӷ���������� */
            }
        }
    }
    
    if ((mode & 0x8F) == 0x80)                                                      /* �޴����㰴�� */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                              /* ֮ǰ�Ǳ����µ� */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                            /* ��ǰ����ɿ� */
        }
        else                                                                        /* ֮ǰ��û�б����� */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                   /* �������Ч��� */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                     /* ���´�10��ʼ���� */
    }
    
    return res;
}
