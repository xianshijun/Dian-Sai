/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       LCD驱动代码
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

#include "lcd.h"
#include "myltdc.h"
#include "lcdfont.h"
#include "lcd_ex.c"
#include "delay.h"
#include "stm32h7xx_hal_sram.h"
SRAM_HandleTypeDef g_sram_handle = {0};   /* SRAM句柄 */

/* 绘制LCD时的背景色 */
uint32_t g_back_color = 0xFFFF;

/* LCD重要参数 */
_lcd_dev lcddev;

/**
 * @brief   LCD写数据
 * @param   data: 要写入的数据
 * @retval  无
 */
void lcd_wr_data(volatile uint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCD写寄存器编号或地址
 * @param   regno: 寄存器编号或地址
 * @retval  无
 */
void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;
    LCD->LCD_REG = regno;
}

/**
 * @brief   LCD写寄存器
 * @param   regno: 寄存器编号
 * @param   data : 要写入的数据
 * @retval  无
 */
void lcd_write_reg(uint16_t regno, uint16_t data)
{
    LCD->LCD_REG = regno;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCD读数据
 * @param   无
 * @retval  读取到的数据
 */
static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;
    
    ram = LCD->LCD_RAM;
    
    return ram;
}

/**
 * @brief   LCD延时函数
 * @note    仅用于部分在-O1时间优化时需要设置的地方
 * @param   t: 延时的数值
 * @retval  无
 */
static void lcd_opt_delay(uint32_t i)
{
    /* 使用AC6时空循环可能被优化，可使用while(1) __asm volatile(""); */
    while (i--);
}

/**
 * @brief   准备写GRAM
 * @param   无
 * @retval  无
 */
void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/**
 * @brief   读取个某点的颜色值
 * @param   x: 指定点的X坐标
 * @param   y: 指定点的Y坐标
 * @retval  指定点的颜色（32位颜色，方便兼容LTDC）
 */
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
    
    if ((x >= lcddev.width) || (y >= (lcddev.height)))          /* 判断点的坐标是否合法 */
    {
        return 0;
    }
    
    if (lcdltdc.pwidth != 0)                                    /* RGB屏 */
    {
        return ltdc_read_point(x, y);
    }
    
    lcd_set_cursor(x, y);                                       /* 设置光标 */
    
    if (lcddev.id == 0x5510)                                    /* 5510 */
    {
        lcd_wr_regno(0x2E00);
    }
    else
    {
        lcd_wr_regno(0x2E);                                     /* 9341/5310/1963/7789/7796/9806 */
    }
    
    r = lcd_rd_data();                                          /* 无效数据（除1963） */
    if (lcddev.id == 0x1963)                                    /* 1963 */
    {
        return r;                                               /* 1963直接读出数据即可 */
    }
    
    lcd_opt_delay(2);
    r = lcd_rd_data();
    if (lcddev.id == 0x7796)                                    /* 7796一次读取一个像素值 */
    {
        return r;
    }

    lcd_opt_delay(2);
    b = lcd_rd_data();                                          /* 5510/9341/5310/7789第一次读出RG数据，第二次读取BR（R是下一个像素的颜色数据）数据 */
    g = (r & 0xFF) << 8;
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* RGB565 */
}

/**
 * @brief   LCD开启显示
 * @param   无
 * @retval  无
 */
void lcd_display_on(void)
{
    if (lcdltdc.pwidth != 0)        /* RGB屏 */
    {
        ltdc_switch(1);
    }
    else if (lcddev.id == 0x5510)   /* 5510 */
    {
        lcd_wr_regno(0x2900);
    }
    else                            /* 9341/5310/1963/7789/7796/9806 */
    {
        lcd_wr_regno(0x29);
    }
}

/**
 * @brief   LCD关闭显示
 * @param   无
 * @retval  无
 */
void lcd_display_off(void)
{
    if (lcdltdc.pwidth != 0)        /* RGB屏 */
    {
        ltdc_switch(0);
    }
    else if (lcddev.id == 0x5510)   /* 5510 */
    {
        lcd_wr_regno(0x2800);
    }
    else                            /* 9341/5310/1963/7789/7796/9806 */
    {
        lcd_wr_regno(0x28);
    }
}

/**
 * @brief   设置光标位置（对RGB屏无效）
 * @param   x: 光标的X坐标
 * @param   y: 光标的Y坐标
 * @retval  无
 */
void lcd_set_cursor(uint16_t x, uint16_t y)
{
    if (lcddev.id == 0x1963)        /* 1963 */
    {
        if (lcddev.dir == 0)        /* 竖屏模式需要变换X坐标 */
        {
            x = lcddev.width - 1 - x;
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(0);
            lcd_wr_data(0);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
        }
        else                        /* 横屏模式 */
        {
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
            lcd_wr_data((lcddev.width - 1) >> 8);
            lcd_wr_data((lcddev.width - 1) & 0xFF);
        }
        
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else if (lcddev.id == 0x5510)   /* 5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(y & 0xFF);
    }
    else                            /* 9341/5310/7789/7796/9806 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
    }
}

/**
 * @brief   设置LCD的自动扫描方向（对RGB屏无效）
 * @note    9341/5310/5510/1963/7789/7796/9806等IC已经实际测试
 *          注意:其他函数可能会受到此函数设置的影响（尤其是9341），
 *          所以，一般设置为L2R_U2D即可，如果设置为其他扫描方式，可能导致显示不正常。
 * @param   dir: LCD扫描方向
 *   @arg   L2R_U2D: 从左到右，从上到下
 *   @arg   L2R_D2U: 从左到右，从下到上
 *   @arg   R2L_U2D: 从右到左，从上到下
 *   @arg   R2L_D2U: 从右到左，从下到上
 *   @arg   U2D_L2R: 从上到下，从左到右
 *   @arg   U2D_R2L: 从上到下，从右到左
 *   @arg   D2U_L2R: 从下到上，从左到右
 *   @arg   D2U_R2L: 从下到上，从右到左
 * @retval  无
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg;
    uint16_t temp;
    
    /* 横屏时，1963不改变扫描方向，其他IC改变扫描方向
     * 竖屏时，1963改变扫描方向，其他IC不改变扫描方向
     */
    if (((lcddev.dir == 1) && (lcddev.id != 0x1963)) || ((lcddev.dir == 0) && (lcddev.id == 0x1963)))
    {
        switch (dir)   /* 方向转换 */
        {
            case L2R_U2D:
            {
                dir = D2U_L2R;
                break;
            }
            case L2R_D2U:
            {
                dir = D2U_R2L;
                break;
            }
            case R2L_U2D:
            {
                dir = U2D_L2R;
                break;
            }
            case R2L_D2U:
            {
                dir = U2D_R2L;
                break;
            }
            case U2D_L2R:
            {
                dir = L2R_D2U;
                break;
            }
            case U2D_R2L:
            {
                dir = L2R_U2D;
                break;
            }
            case D2U_L2R:
            {
                dir = R2L_D2U;
                break;
            }
            case D2U_R2L:
            {
                dir = R2L_U2D;
                break;
            }
        }
    }
    
    /* 根据扫描方向设置0x36或0x3600寄存器bit5~7位的值 */
    switch (dir)
    {
        case L2R_U2D:
        {
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        }
        case L2R_D2U:
        {
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        }
        case R2L_U2D:
        {
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        }
        case R2L_D2U:
        {
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        }
        case U2D_L2R:
        {
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        }
        case U2D_R2L:
        {
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        }
        case D2U_L2R:
        {
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        }
        case D2U_R2L:
        {
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
        }
    }
    
    if (lcddev.id == 0x5510)                    /* 0x5510 */
    {
        dirreg = 0x3600;
    }
    else                                        /* 9341/5310/1963/7789 */
    {
        dirreg = 0x36;
    }
    
    /* 9341、7789和7796要设置BGR位 */
    if ((lcddev.id == 0x9341) || (lcddev.id == 0x7789) || (lcddev.id == 0x7796))
    {
        regval |= 0x08;
    }
    
    lcd_write_reg(dirreg, regval);
    
    if (lcddev.id != 0x1963)                    /* 1963不用做坐标处理 */
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)   /* 交换X和Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   /* 交换X和Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }
    
    /* 设置显示区域（开窗）大小 */
    if (lcddev.id == 0x5510)                    /* 0x5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else                                        /* 9341/5310/1963/7789 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
}

/**
 * @brief   画点
 * @param   x: 点的X坐标
 * @param   y: 点的Y坐标
 * @param   color: 点的颜色（32位颜色，方便兼容LTDC）
 * @retval  无
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    if (lcdltdc.pwidth != 0)        /* RGB屏 */
    {
        ltdc_draw_point(x, y, color);
    }
    else
    {
        lcd_set_cursor(x, y);       /* 设置光标位置 */
        lcd_write_ram_prepare();    /* 开始写入GRAM */
        LCD->LCD_RAM = color;
    }
}

/**
 * @brief   设置SSD1963背光亮度
 * @param   pwm: 背光等级，范围0~100（数值越大越亮）
 * @retval  无
 */
void lcd_ssd_backlight_set(uint8_t pwm)
{
    lcd_wr_regno(0xBE);         /* 配置PWM输出 */
    lcd_wr_data(0x05);          /* 设置PWM频率 */
    lcd_wr_data(pwm * 2.55);    /* 设置PWM占空比 */
    lcd_wr_data(0x01);          /* 设置C */
    lcd_wr_data(0xFF);          /* 设置D */
    lcd_wr_data(0x00);          /* 设置E */
    lcd_wr_data(0x00);          /* 设置F */
}

/**
 * @brief   设置LCD显示方向
 * @param   dir: LCD显示方向
 *   @arg   0: 竖屏
 *   @arg   1: 横屏
 * @retval  无
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;
    
    if (lcdltdc.pwidth != 0)                                        /* RGB屏 */
    {
        ltdc_display_dir(dir);
        lcddev.width = lcdltdc.width;
        lcddev.height = lcdltdc.height;
        return;
    }
    
    if (dir == 0)                                                   /* 竖屏 */
    {
        lcddev.width = 240;
        lcddev.height = 320;
        
        if (lcddev.id == 0x5510)                                    /* 5510 */
        {
            lcddev.wramcmd = 0x2C00;                                /* 开始写GRAM指令 */
            lcddev.setxcmd = 0x2A00;                                /* 设置X坐标指令 */
            lcddev.setycmd = 0x2B00;                                /* 设置Y坐标指令 */
            lcddev.width = 480;                                     /* LCD宽度 */
            lcddev.height = 800;                                    /* LCD高度 */
        }
        else if (lcddev.id == 0x1963)                               /* 1963 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2B;
            lcddev.setycmd = 0x2A;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else                                                        /* 9341/5310/7789/7796/9806 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }
        
        if ((lcddev.id == 0x5310) || (lcddev.id == 0x7796))         /* 5310/7796 */
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }

        if (lcddev.id == 0x9806)
        {
            lcddev.width = 480;
            lcddev.height = 800;
        }
    }
    else                                                            /* 横屏 */
    {
        lcddev.width = 320;
        lcddev.height = 240;
        
        if (lcddev.id == 0x5510)                                    /* 5510 */
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if ((lcddev.id == 0x1963) || (lcddev.id == 0x9806))    /* 1963/9806 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else                                                        /* 9341/5310/7789/7796 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }
        
        if ((lcddev.id == 0x5310) || (lcddev.id == 0x7796))         /* 5310/7796 */
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    
    lcd_scan_dir(DFT_SCAN_DIR);         /* 设置LCD为默认扫描方向 */
}

/**
 * @brief   设置窗口（对RGB屏无效）
 * @note    会自动设置画点坐标到窗口左上角(sx,sy)
 * @param   sx    : 窗口起始X坐标
 * @param   sy    : 窗口起始Y坐标
 * @param   width : 窗口宽度，需大于0
 * @param   height: 窗口高度，需大于0
 *  @note   窗口大小 = width * height
 * @retval  无
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth;
    uint16_t theight;
    
    twidth = sx + width - 1;
    theight = sy + height - 1;
    
    if (lcdltdc.pwidth != 0)                        /* RGB屏 */
    {
        return;
    }
    
    if ((lcddev.id == 0x1963) && (lcddev.dir != 1)) /* 1963（竖屏） */
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data((sx + width - 1) >> 8);
        lcd_wr_data((sx + width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(height >> 8);
        lcd_wr_data(height & 0xFF);
    }
    else if (lcddev.id == 0x5510)                   /* 5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data(twidth >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data(theight >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data(theight & 0xFF);
    }
    else                                            /* 9341/5310/1963（横屏）/7789 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data(twidth >> 8);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(theight >> 8);
        lcd_wr_data(theight & 0xFF);
    }
}

/**
 * @brief   清屏
 * @param   color: 清屏的颜色
 * @retval  无
 */
void lcd_clear(uint16_t color)
{
    uint32_t index;
    uint32_t totalpoint;
    
    if (lcdltdc.pwidth != 0)                        /* RGB屏 */
    {
        ltdc_clear(color);
    }
    else
    {
        totalpoint = lcddev.width * lcddev.height;  /* 计算总像素数量 */
        lcd_set_cursor(0x00, 0x0000);               /* 设置光标位置 */
        lcd_write_ram_prepare();                    /* 开始写入GRAM */
        for (index=0; index<totalpoint; index++)
        {
            LCD->LCD_RAM = color;
        }
    }
}

/**
 * @brief   在指定区域内填充单个颜色
 * @param   sx    : 指定区域的起始X坐标
 * @param   sy    : 指定区域的起始Y坐标
 * @param   ex    : 指定区域的结束X坐标
 * @param   ey    : 指定区域的结束Y坐标
 * @param   color : 要填充的颜色（32位颜色，方便兼容LTDC）
 *  @note   指定区域的大小 = (ex - sx + 1) * (ey - sy + 1)
 * @retval  无
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i;
    uint16_t j;
    uint16_t xlen;
    
    
    if (lcdltdc.pwidth != 0)            /* RGB屏 */
    {
        ltdc_fill(sx, sy, ex, ey, color);
    }
    else
    {
        xlen = ex - sx + 1;
        for (i=sy; i<=ey; i++)
        {
            lcd_set_cursor(sx, i);      /* 设置光标位置 */
            lcd_write_ram_prepare();    /* 开始写入GRAM */
            for (j=0; j<xlen; j++)
            {
                LCD->LCD_RAM = color;
            }
        }
    }
}

/**
 * @brief   在指定区域内填充指定颜色块
 * @param   sx    : 指定区域的起始X坐标
 * @param   sy    : 指定区域的起始Y坐标
 * @param   ex    : 指定区域的结束X坐标
 * @param   ey    : 指定区域的结束Y坐标
 * @param   color : 指定颜色数组的首地址
 *  @note   指定区域的大小 = (ex - sx + 1) * (ey - sy + 1)
 * @retval  无
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height;
    uint16_t width;
    uint16_t i;
    uint16_t j;
    
    if (lcdltdc.pwidth != 0)            /* RGB屏 */
    {
        ltdc_color_fill(sx, sy, ex, ey, color);
    }
    else
    {
        width = ex - sx + 1;            /* 计算指定区域的宽度 */
        height = ey - sy + 1;           /* 计算指定区域的高度 */
        for (i=0; i<height; i++)
        {
            lcd_set_cursor(sx, sy + i); /* 设置光标位置 */
            lcd_write_ram_prepare();    /* 开始写入GRAM */
            for (j=0; j<width; j++)
            {
                LCD->LCD_RAM = color[i * width + j];
            }
        }
    }
}

/**
 * @brief   画线
 * @param   x1   : 线的起始X坐标
 * @param   y1   : 线的起始Y坐标
 * @param   x2   : 线的结束X坐标
 * @param   y2   : 线的结束Y坐标
 * @param   color: 线的颜色
 * @retval  无
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx;
    int incy;
    int row;
    int col;
    
    /* 计算坐标增量 */
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    
    row = x1;
    col = y1;
    
    /* 设置X单步方向 */
    if (delta_x > 0)
    {
        incx = 1;
    }
    else if (delta_x == 0)
    {
        incx = 0;
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    /* 设置Y单步方向 */
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    /* 选取基本增量坐标轴 */
    if (delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }
    
    for (t=0; t<=(distance+1); t++)
    {
        lcd_draw_point(row, col, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief   画水平线
 * @param   x    : 线的起始X坐标
 * @param   y    : 线的起始Y坐标
 * @param   len  : 线的长度
 * @param   color: 线的颜色
 * @retval  无
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))
    {
        return;
    }
    
    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief   画矩形
 * @param   x1   : 矩形左上角X坐标
 * @param   y1   : 矩形左上角Y坐标
 * @param   x2   : 矩形右下角X坐标
 * @param   y2   : 矩形右下角Y坐标
 * @param   color: 矩形的颜色
 * @retval  无
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief   画圆
 * @param   x0   : 圆心的X坐标
 * @param   y0   : 圆心的Y坐标
 * @param   r    : 圆的半径
 * @param   color: 圆的颜色
 * @retval  无
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a;
    int b;
    int di;
    
    a = 0;
    b = r;
    di = 3 - (r << 1);  /* 判断下个点位置的标志 */
    
    while (a <= b)      /* 使用Bresenham算法画圆 */
    {
        lcd_draw_point(x0 + a, y0 - b, color);
        lcd_draw_point(x0 + b, y0 - a, color);
        lcd_draw_point(x0 + b, y0 + a, color);
        lcd_draw_point(x0 + a, y0 + b, color);
        lcd_draw_point(x0 - a, y0 + b, color);
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);
        lcd_draw_point(x0 - b, y0 - a, color);
        a++;
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief   画实心圆
 * @param   x    : 圆心的X坐标
 * @param   y    : 圆心的Y坐标
 * @param   r    : 圆的半径
 * @param   color: 圆的颜色
 * @retval  无
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax;
    uint32_t sqmax;
    uint32_t xr;
    
    imax = ((uint32_t)r * 707) / 1000 + 1;
    sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    xr = r;
    
    lcd_draw_hline(x - r, y, 2 * r, color);
    for (i=1; i<=imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }
            xr--;
        }
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief   在指定位置显示一个字符
 * @param   x    : 指定位置的X坐标
 * @param   y    : 指定位置的Y坐标
 * @param   chr  : 要显示的字符，范围：' '~'~'
 * @param   size : 字体
 *   @arg   12: 12*12 ASCII字符
 *   @arg   16: 16*16 ASCII字符
 *   @arg   24: 24*24 ASCII字符
 *   @arg   32: 32*32 ASCII字符
 * @param   mode : 显示模式
 *   @arg   0: 非叠加方式
 *   @arg   1: 叠加方式
 * @param   color: 字符的颜色
 * @retval  无
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t csize;
    uint8_t *pfont;
    uint16_t y0;
    uint8_t t;
    uint8_t t1;
    uint8_t temp;
    
    csize = ((size >> 3) + (((size & 0x7) != 0) ? 1 : 0)) * (size >> 1);    /* 计算所选字体对应一个字符所占的字节数 */
    chr -= ' ';                                                             /* 计算偏移后的值，因为字库是从空格开始的 */
    
    switch (size)
    {
        case 12:                                                            /* 12*12 ASCII字符 */
        {
            pfont = (uint8_t *)asc2_1206[chr];
            break;
        }
        case 16:                                                            /* 16*16 ASCII字符 */
        {
            pfont = (uint8_t *)asc2_1608[chr];
            break;
        }
        case 24:                                                            /* 24*24 ASCII字符 */
        {
            pfont = (uint8_t *)asc2_2412[chr];
            break;
        }
        case 32:                                                            /* 32*32 ASCII字符 */
        {
            pfont = (uint8_t *)asc2_3216[chr];
            break;
        }
        default:
        {
            return;
        }
    }
    
    y0 = y;
    for (t=0; t<csize; t++)
    {
        temp = pfont[t];                                                    /* 获取字符的点阵数据 */
        for (t1=0; t1<8; t1++)                                              /* 遍历一个字节的8个位 */
        {
            if ((temp & 0x80) != 0)                                         /* 需要显示的有效点 */
            {
                lcd_draw_point(x, y, color);                                /* 以字符颜色绘制这个点 */
            }
            else if (mode == 0)                                             /* 不需要显示的无效点 */
            {
                lcd_draw_point(x, y, g_back_color);                         /* 绘制背景色 */
            }
            
            temp <<= 1;                                                     /* 移位至下一个位 */
            y++;
            if (y >= lcddev.height)                                         /* 判断Y坐标是否超出显示区域 */
            {
                return;
            }
            if ((y - y0) == size)                                           /* 本行绘制完成 */
            {
                y = y0;                                                     /* Y坐标复位 */
                x++;                                                        /* 下一行 */
                if (x >= lcddev.width)                                      /* 判断X坐标是否超出显示区域 */
                {
                    return;
                }
                break;
            }
        }
    }
}

/**
 * @brief   平方函数
 * @param   m: 底数
 * @param   n: 指数
 * @retval  m^n
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    
    while (n--)
    {
        result *= m;
    }
    
    return result;
}

/**
 * @brief   显示len个数字
 * @param   x    : 起始X坐标
 * @param   y    : 起始Y坐标
 * @param   num  : 数值，范围：0~2^32
 * @param   len  : 显示数字的位数
 * @param   size : 字体
 *   @arg   12: 12*12 ASCII字符
 *   @arg   16: 16*16 ASCII字符
 *   @arg   24: 24*24 ASCII字符
 *   @arg   32: 32*32 ASCII字符
 * @param   color: 数字的颜色
 * @retval  无
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t;
    uint8_t temp;
    uint8_t enshow = 0;
    
    for (t=0; t<len; t++)                                                   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                       /* 获取对应位的数字 */
        if((enshow == 0) && (t < (len - 1)))                                /* 没有使能显示，且还有位要显示 */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size >> 1) * t, y, ' ', size, 0, color); /* 显示空格，占位 */
                continue;                                                   /* 继续下一个位 */
            }
            else
            {
                enshow = 1;                                                 /* 使能显示 */
            }
        }
        
        lcd_show_char(x + (size >> 1) * t, y, temp + '0', size, 0, color);  /* 显示字符 */
    }
}

/**
 * @brief   扩展显示len个数字（显示高位0）
 * @param   x    : 起始X坐标
 * @param   y    : 起始Y坐标
 * @param   num  : 数值，范围：0~2^32
 * @param   len  : 显示数字的位数
 * @param   size : 字体
 *   @arg   12: 12*12 ASCII字符
 *   @arg   16: 16*16 ASCII字符
 *   @arg   24: 24*24 ASCII字符
 *   @arg   32: 32*32 ASCII字符
 * @param   mode : 显示模式
 *   @arg   0: 非叠加方式
 *   @arg   1: 叠加方式
 * @param   color: 数字的颜色
 * @retval  无
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t;
    uint8_t temp;
    uint8_t enshow = 0;
    
    for (t=0; t<len; t++)                                                                   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                                       /* 获取对应位的数字 */
        if((enshow == 0) && (t < (len - 1)))                                                /* 没有使能显示，且还有位要显示 */
        {
            if (temp == 0)
            {
                if ((mode & 0x80) != 0)                                                     /* 高位需要填充0 */
                {
                    lcd_show_char(x + (size >> 1) * t, y, '0', size, mode & 0x01, color);   /* 显示0，占位 */
                }
                else
                {
                    lcd_show_char(x + (size >> 1) * t, y, ' ', size, mode & 0x01, color);   /* 显示空格，占位 */
                }
                continue;                                                                   /* 继续下一个位 */
            }
            else
            {
                enshow = 1;                                                                 /* 使能显示 */
            }
        }
        
        lcd_show_char(x + (size >> 1) * t, y, temp + '0', size, mode & 0x01, color);        /* 显示字符 */
    }
}

/**
 * @brief   显示字符串
 * @param   x     : 起始X坐标
 * @param   y     : 起始Y坐标
 * @param   width : 显示区域宽度
 * @param   height: 显示区域高度
 * @param   size  : 字体
 *   @arg   12: 12*12 ASCII字符
 *   @arg   16: 16*16 ASCII字符
 *   @arg   24: 24*24 ASCII字符
 *   @arg   32: 32*32 ASCII字符
 * @param   *p    : 字符串指针
 * @param   color : 字符串的颜色
 * @retval  无
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0;
    
    x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' '))              /* 判断是否为非法字符 */
    {
        if (x >= width)                             /* 宽度越界 */
        {
            x = x0;                                 /* 换行 */
            y += size;
        }
        
        if (y >= height)                            /* 高度越界 */
        {
            break;                                  /* 退出 */
        }
        
        lcd_show_char(x, y, *p, size, 0, color);    /* 显示一个字符 */
        x += (size >> 1);                           /* ASCII字符宽度为高度的一半 */
        p++;
    }
}

/**
 * @brief   初始化LCD
 * @param   无
 * @retval  无
 */
void lcd_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct;
//    FMC_NORSRAM_TimingTypeDef fmc_read_timing_struct = {0};
//    FMC_NORSRAM_TimingTypeDef fmc_write_timing_struct = {0};
    
//    /* 多次读取，避免上电时IO电平波动带来的干扰 */
    lcddev.id = ltdc_panelid_read();
    lcddev.id = ltdc_panelid_read();

    if (lcddev.id != 0)
    {
        ltdc_init();
    }
    else
    {
//        /* 配置时钟 */
//        rcc_periph_clk_init_struct.PeriphClockSelection |= RCC_PERIPHCLK_FMC;
//        rcc_periph_clk_init_struct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
//        HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct);
//        
//        /* 使能时钟 */
//        LCD_BL_GPIO_CLK_ENABLE();
//        
//        /* 配置LCD BL引脚 */
//        gpio_init_struct.Pin = LCD_BL_GPIO_PIN;
//        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
//        gpio_init_struct.Pull = GPIO_PULLUP;
//        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//        HAL_GPIO_Init(LCD_BL_GPIO_PORT, &gpio_init_struct);
//        
//        /* 配置FMC读时序 */
//        fmc_read_timing_struct.AddressSetupTime = 15;
//        fmc_read_timing_struct.AddressHoldTime = 0;
//        fmc_read_timing_struct.DataSetupTime = 78;
//        fmc_read_timing_struct.AccessMode = FMC_ACCESS_MODE_A;
//        
//        /* 配置FMC写时序 */
//        fmc_write_timing_struct.AddressSetupTime = 15;
//        fmc_write_timing_struct.AddressHoldTime = 0;
//        fmc_write_timing_struct.DataSetupTime = 15;
//        fmc_write_timing_struct.AccessMode = FMC_ACCESS_MODE_A;
//        
//        /* 配置FMC */
//        g_sram_handle.Instance = FMC_NORSRAM_DEVICE;
//        g_sram_handle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
//        g_sram_handle.Init.NSBank = FMC_NORSRAM_BANK1;
//        g_sram_handle.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
//        g_sram_handle.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
//        g_sram_handle.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
//        g_sram_handle.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
//        g_sram_handle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
//        g_sram_handle.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
//        g_sram_handle.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
//        g_sram_handle.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
//        g_sram_handle.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
//        g_sram_handle.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
//        g_sram_handle.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
//        g_sram_handle.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
//        g_sram_handle.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
//        g_sram_handle.Init.PageSize = FMC_PAGE_SIZE_NONE;
//        
////        HAL_SRAM_Init(&g_sram_handle, &fmc_read_timing_struct, &fmc_write_timing_struct);
//        delay_ms(50);
//        
//        /* 尝试读取ILI9341的ID */
//        lcd_wr_regno(0xD3);
//        lcddev.id = lcd_rd_data();                                                              /* 无效数据 */
//        lcddev.id = lcd_rd_data();                                                              /* 读取到0x00 */
//        lcddev.id = lcd_rd_data() << 8;                                                         /* 读取到0x93 */
//        lcddev.id |= lcd_rd_data();                                                             /* 读取到0x41 */
//        if (lcddev.id != 0x9341)                                                                /* 尝试读取ST7789的ID */
//        {
//            lcd_wr_regno(0x04);
//            lcddev.id = lcd_rd_data();                                                          /* 无效数据 */
//            lcddev.id = lcd_rd_data();                                                          /* 读取到0x85 */
//            lcddev.id = lcd_rd_data() << 8;                                                     /* 读取到0x85 */
//            lcddev.id |= lcd_rd_data();                                                         /* 读取到0x52 */
//            if (lcddev.id == 0x8552)                                                            /* 将ST7789的ID记为0x7789 */
//            {
//                lcddev.id = 0x7789;
//            }
//            if (lcddev.id != 0x7789)                                                            /* 尝试读取NT35310的ID */
//            {
//                lcd_wr_regno(0xD4);
//                lcddev.id = lcd_rd_data();                                                      /* 无效数据 */
//                lcddev.id = lcd_rd_data();                                                      /* 读取到0x01 */
//                lcddev.id = lcd_rd_data() << 8;                                                 /* 读取到0x53 */
//                lcddev.id |= lcd_rd_data();                                                     /* 读取到0x10 */
//                if (lcddev.id != 0x5310)                                                        /* 尝试读取ST7796的ID */
//                {
//                    lcd_wr_regno(0XD3);
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id <<= 8;
//                    lcddev.id |= lcd_rd_data();
//                    if (lcddev.id != 0x7796)                                                    /* 尝试读取NT35510的ID */
//                    {
//                        /* 发送密钥（厂家提供） */
//                        lcd_write_reg(0xF000, 0x0055);
//                        lcd_write_reg(0xF001, 0x00AA);
//                        lcd_write_reg(0xF002, 0x0052);
//                        lcd_write_reg(0xF003, 0x0008);
//                        lcd_write_reg(0xF004, 0x0001);
//                        
//                        lcd_wr_regno(0xC500);
//                        lcddev.id = lcd_rd_data() << 8;                                         /* 读取到0x80 */
//                        lcd_wr_regno(0xC501);
//                        lcddev.id |= lcd_rd_data();                                             /* 读取到0x00 */
//                        delay_ms(5);                                                            /* 因为此NT5510的指令恰好为SSD1963的软件复位指令 */
//                        if (lcddev.id != 0x5510)                                                /* 尝试读取ILI9806的ID */
//                        {
//                            lcd_wr_regno(0XD3);
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id <<= 8;
//                            lcddev.id |= lcd_rd_data();
//                            if (lcddev.id != 0x9806)                                            /* 尝试读取SSD1963的ID */
//                            {
//                                lcd_wr_regno(0xA1);
//                                lcddev.id = lcd_rd_data();
//                                lcddev.id = lcd_rd_data() << 8;                                 /* 读取到0x57 */
//                                lcddev.id |= lcd_rd_data();                                     /* 读取到0x61 */
//                                if (lcddev.id == 0x5761)                                        /* 将SSD1963的ID记为0x1963 */
//                                {
//                                    lcddev.id = 0x1963;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        
//        if (lcddev.id == 0x7789)
//        {
//            lcd_ex_st7789_reginit();                                                            /* 执行ST7789初始化 */
//        }
//        else if (lcddev.id == 0x9341)
//        {
//            lcd_ex_ili9341_reginit();                                                           /* 执行ILI9341初始化 */
//        }
//        else if (lcddev.id == 0x5310)
//        {
//            lcd_ex_nt35310_reginit();                                                           /* 执行NT35310初始化 */
//        }
//        else if (lcddev.id == 0x7796)
//        {
//            lcd_ex_st7796_reginit();                                                            /* 执行ST7796初始化 */
//        }
//        else if (lcddev.id == 0x5510)
//        {
//            lcd_ex_nt35510_reginit();                                                           /* 执行NT35510初始化 */
//        }
//        else if (lcddev.id == 0x9806)
//        {
//            lcd_ex_ili9806_reginit();                                                           /* 执行ILI9806初始化 */
//        }
//        else if (lcddev.id == 0x1963)
//        {
//            lcd_ex_ssd1963_reginit();                                                           /* 执行SSD1963初始化 */
//            lcd_ssd_backlight_set(100);                                                         /* 背光设置为最亮 */
//        }
//        
//        /* 初始化完成后，提速 */
//        if ((lcddev.id == 0x9341) || (lcddev.id == 0x1963) || (lcddev.id == 0x7789))            /* 9341/1963/7789 */
//        {
//            fmc_write_timing_struct.AddressSetupTime = 5;
//            fmc_write_timing_struct.DataSetupTime = 5;
//            FMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fmc_write_timing_struct, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);
//        }
//        else if ((lcddev.id == 0x5310) ||                                                       /* 5310/7796/5510/9806 */
//                 (lcddev.id == 0x7796) ||
//                 (lcddev.id == 0x5510) ||
//                 (lcddev.id == 0x9806))
//        {
//            fmc_write_timing_struct.AddressSetupTime = 3;
//            fmc_write_timing_struct.DataSetupTime = 3;
//            FMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fmc_write_timing_struct, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);
//        }
    }
    
    lcd_display_dir(1);                                                                         /* 默认设置为竖屏 */
    LCD_BL(1);                                                                                  /* 点亮背光 */
    lcd_clear(WHITE);                                                                           /* 清屏 */
		printf("%s\r\n",__FUNCTION__);
}

///**
// * @brief   HAL库SRAM初始化MSP函数
// * @param   无
// * @retval  无
// */
//void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    
//    if (hsram->Instance == FMC_NORSRAM_DEVICE)
//    {
//        /* 使能时钟 */
//        __HAL_RCC_FMC_CLK_ENABLE();
//        __HAL_RCC_GPIOD_CLK_ENABLE();
//        __HAL_RCC_GPIOE_CLK_ENABLE();
//        LCD_WR_GPIO_CLK_ENABLE();
//        LCD_RD_GPIO_CLK_ENABLE();
//        LCD_CS_GPIO_CLK_ENABLE();
//        LCD_RS_GPIO_CLK_ENABLE();
//        
//        /* 配置FMC引脚 */
//        gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
//        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
//        gpio_init_struct.Pull = GPIO_PULLUP;
//        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//        gpio_init_struct.Alternate = GPIO_AF12_FMC;
//        HAL_GPIO_Init(GPIOD, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//        HAL_GPIO_Init(GPIOE, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = LCD_WR_GPIO_PIN;
//        gpio_init_struct.Alternate = LCD_WR_GPIO_AF;
//        HAL_GPIO_Init(LCD_WR_GPIO_PORT, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = LCD_RD_GPIO_PIN;
//        gpio_init_struct.Alternate = LCD_RD_GPIO_AF;
//        HAL_GPIO_Init(LCD_RD_GPIO_PORT, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
//        gpio_init_struct.Alternate = LCD_CS_GPIO_AF;
//        HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);
//        
//        gpio_init_struct.Pin = LCD_RS_GPIO_PIN;
//        gpio_init_struct.Alternate = LCD_RS_GPIO_AF;
//        HAL_GPIO_Init(LCD_RS_GPIO_PORT, &gpio_init_struct);
//    }
//}
