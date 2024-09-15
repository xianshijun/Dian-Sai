/**
 ****************************************************************************************************
 * @file        ft5206.c
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

#include "ft5206.h"
#include "delay.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"

/**
 * @brief   从FT5206读出一次数据
 * @param   reg: 起始寄存器地址
 * @param   buf: 数据保存起始地址
 * @param   len: 待读出数据的长度
 * @retval  无
 */
static void ft5206_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);                        /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* 发送寄存器低8位地址 */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_RD);                        /* 发送读命令 */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* 循环读出数据 */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief   从FT5206写入一次数据
 * @param   reg: 起始寄存器地址
 * @param   buf: 待写入数据的起始地址
 * @param   len: 待写入数据的长度
 * @retval  写入结果
 * @arg     0: 成功
 * @arg     1: 失败
 */
static uint8_t ft5206_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);       /* 发送寄存器低8位地址 */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)           /* 循环写入数据 */
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
 * @brief   初始化FT5206
 * @param   无
 * @retval  初始化结果
 *          0: 成功
 *          1: 失败
 */
uint8_t ft5206_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp[2];
    
    /* 时钟使能 */
    FT5206_RST_GPIO_CLK_ENABLE();
    FT5206_INT_GPIO_CLK_ENABLE();
    
    /* 配置RST引脚 */
    gpio_init_struct.Pin = FT5206_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_RST_GPIO_PORT, &gpio_init_struct);
    
    /* 配置INT引脚 */
    gpio_init_struct.Pin = FT5206_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_INT_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化触摸屏IIC */
    ct_iic_init();
    
    /* 复位FT5206 */
    FT5206_RST(0);
    delay_ms(20);
    FT5206_RST(1);
    delay_ms(50);
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_DEVIDE_MODE, temp, 1);             /* 配置进入正常操作模式 */
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_ID_G_MODE, temp, 1);               /* 查询模式 */
    
    temp[0] = 22;
    ft5206_wr_reg(FT5206_ID_G_THGROUP, temp, 1);            /* 触摸有效值，越小越灵敏 */
    
    temp[0] = 12;
    ft5206_wr_reg(FT5206_ID_G_PERIODACTIVE, temp, 1);       /* 激活周期，不能小于12，最大14 */
    
    ft5206_rd_reg(FT5206_ID_G_LIB_VERSION, temp, 2);        /* 读取版本号 */
    if (((temp[0] == 0x30) && (temp[1] == 0x03)) ||         /* 0x3003 */
        ((temp[0] == 0x00) && (temp[1] == 0x01)) ||         /* 0x0001 */
        ((temp[0] == 0x00) && (temp[1] == 0x02)) ||         /* 0x0002 */
        ((temp[0] == 0x00) && (temp[1] == 0x00)))           /* CST340 */
    {
        return 0;
    }
    
    return 1;
}

/* FT5206读取触摸坐标寄存器 */
static const uint16_t FT5206_TPX_TBL[5] = {
    FT5206_TP1_REG,
    FT5206_TP2_REG,
    FT5206_TP3_REG,
    FT5206_TP4_REG,
    FT5206_TP5_REG,
};

/**
 * @brief   扫描FT5206读触摸屏
 * @note    采用查询方式
 * @param   mode: 电容屏未使用，用于兼容电阻屏
 * @retval  当前触摸状态
 * @arg     0: 无触摸
 * @arg     1: 有触摸
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
        ft5206_rd_reg(FT5206_REG_NUM_FINGER, &mode, 1);                                             /* 读取触摸点的状态 */
        if ((mode & 0x0F) && ((mode & 0x0F) < 6))
        {
            temp = 0xFFFF << (mode & 0x0F);                                                         /* 将点的个数转换为1的位数，匹配tp_dev.sta定义 */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            delay_ms(4);
            
            for (i=0; i<5; i++)
            {
                if (tp_dev.sta & (1 << i))                                                          /* 触摸点有效 */
                {
                    ft5206_rd_reg(FT5206_TPX_TBL[i], buf, 4);                                       /* 读取XY坐标值 */
                    
                    if (tp_dev.touchtype & 0x01)                                                    /* 横屏 */
                    {
                        tp_dev.y[i] = ((uint16_t)(buf[0] & 0x0F) << 8) + buf[1];
                        tp_dev.x[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    else                                                                            /* 竖屏 */
                    {
                        tp_dev.x[i] = lcddev.width - (((uint16_t)(buf[0] & 0x0F) << 8) + buf[1]);
                        tp_dev.y[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    
                    if ((buf[0] & 0xF0) != 0x80)
                    {
                        tp_dev.x[i] = tp_dev.y[i] = 0;                                              /* 必须是contact事件，才认为有效 */
                    }
                }
            }
            res = 1;
            
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0)                                               /* 读到的数据都是0，则忽略此次数据 */
            {
                mode = 0;
            }
            
            t = 0;                                                                                  /* 触发一次，则会最少连续监测10次，从而提高命中率 */
        }
    }
    
    if ((mode & 0x1F) == 0)                                                                         /* 无触摸点按下 */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                                              /* 之前是被按下的 */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                                            /* 标记按键松开 */
        }
        else                                                                                        /* 之前就没有被按下 */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                                   /* 清除点有效标记 */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                                     /* 重新从10开始计数 */
    }
    
    return res;
}
