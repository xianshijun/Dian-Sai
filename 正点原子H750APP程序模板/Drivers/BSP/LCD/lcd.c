/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       LCD��������
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

#include "lcd.h"
#include "myltdc.h"
#include "lcdfont.h"
#include "lcd_ex.c"
#include "delay.h"
#include "stm32h7xx_hal_sram.h"
SRAM_HandleTypeDef g_sram_handle = {0};   /* SRAM��� */

/* ����LCDʱ�ı���ɫ */
uint32_t g_back_color = 0xFFFF;

/* LCD��Ҫ���� */
_lcd_dev lcddev;

/**
 * @brief   LCDд����
 * @param   data: Ҫд�������
 * @retval  ��
 */
void lcd_wr_data(volatile uint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCDд�Ĵ�����Ż��ַ
 * @param   regno: �Ĵ�����Ż��ַ
 * @retval  ��
 */
void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;
    LCD->LCD_REG = regno;
}

/**
 * @brief   LCDд�Ĵ���
 * @param   regno: �Ĵ������
 * @param   data : Ҫд�������
 * @retval  ��
 */
void lcd_write_reg(uint16_t regno, uint16_t data)
{
    LCD->LCD_REG = regno;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCD������
 * @param   ��
 * @retval  ��ȡ��������
 */
static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;
    
    ram = LCD->LCD_RAM;
    
    return ram;
}

/**
 * @brief   LCD��ʱ����
 * @note    �����ڲ�����-O1ʱ���Ż�ʱ��Ҫ���õĵط�
 * @param   t: ��ʱ����ֵ
 * @retval  ��
 */
static void lcd_opt_delay(uint32_t i)
{
    /* ʹ��AC6ʱ��ѭ�����ܱ��Ż�����ʹ��while(1) __asm volatile(""); */
    while (i--);
}

/**
 * @brief   ׼��дGRAM
 * @param   ��
 * @retval  ��
 */
void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/**
 * @brief   ��ȡ��ĳ�����ɫֵ
 * @param   x: ָ�����X����
 * @param   y: ָ�����Y����
 * @retval  ָ�������ɫ��32λ��ɫ���������LTDC��
 */
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
    
    if ((x >= lcddev.width) || (y >= (lcddev.height)))          /* �жϵ�������Ƿ�Ϸ� */
    {
        return 0;
    }
    
    if (lcdltdc.pwidth != 0)                                    /* RGB�� */
    {
        return ltdc_read_point(x, y);
    }
    
    lcd_set_cursor(x, y);                                       /* ���ù�� */
    
    if (lcddev.id == 0x5510)                                    /* 5510 */
    {
        lcd_wr_regno(0x2E00);
    }
    else
    {
        lcd_wr_regno(0x2E);                                     /* 9341/5310/1963/7789/7796/9806 */
    }
    
    r = lcd_rd_data();                                          /* ��Ч���ݣ���1963�� */
    if (lcddev.id == 0x1963)                                    /* 1963 */
    {
        return r;                                               /* 1963ֱ�Ӷ������ݼ��� */
    }
    
    lcd_opt_delay(2);
    r = lcd_rd_data();
    if (lcddev.id == 0x7796)                                    /* 7796һ�ζ�ȡһ������ֵ */
    {
        return r;
    }

    lcd_opt_delay(2);
    b = lcd_rd_data();                                          /* 5510/9341/5310/7789��һ�ζ���RG���ݣ��ڶ��ζ�ȡBR��R����һ�����ص���ɫ���ݣ����� */
    g = (r & 0xFF) << 8;
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* RGB565 */
}

/**
 * @brief   LCD������ʾ
 * @param   ��
 * @retval  ��
 */
void lcd_display_on(void)
{
    if (lcdltdc.pwidth != 0)        /* RGB�� */
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
 * @brief   LCD�ر���ʾ
 * @param   ��
 * @retval  ��
 */
void lcd_display_off(void)
{
    if (lcdltdc.pwidth != 0)        /* RGB�� */
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
 * @brief   ���ù��λ�ã���RGB����Ч��
 * @param   x: ����X����
 * @param   y: ����Y����
 * @retval  ��
 */
void lcd_set_cursor(uint16_t x, uint16_t y)
{
    if (lcddev.id == 0x1963)        /* 1963 */
    {
        if (lcddev.dir == 0)        /* ����ģʽ��Ҫ�任X���� */
        {
            x = lcddev.width - 1 - x;
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(0);
            lcd_wr_data(0);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
        }
        else                        /* ����ģʽ */
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
 * @brief   ����LCD���Զ�ɨ�跽�򣨶�RGB����Ч��
 * @note    9341/5310/5510/1963/7789/7796/9806��IC�Ѿ�ʵ�ʲ���
 *          ע��:�����������ܻ��ܵ��˺������õ�Ӱ�죨������9341����
 *          ���ԣ�һ������ΪL2R_U2D���ɣ��������Ϊ����ɨ�跽ʽ�����ܵ�����ʾ��������
 * @param   dir: LCDɨ�跽��
 *   @arg   L2R_U2D: �����ң����ϵ���
 *   @arg   L2R_D2U: �����ң����µ���
 *   @arg   R2L_U2D: ���ҵ��󣬴��ϵ���
 *   @arg   R2L_D2U: ���ҵ��󣬴��µ���
 *   @arg   U2D_L2R: ���ϵ��£�������
 *   @arg   U2D_R2L: ���ϵ��£����ҵ���
 *   @arg   D2U_L2R: ���µ��ϣ�������
 *   @arg   D2U_R2L: ���µ��ϣ����ҵ���
 * @retval  ��
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg;
    uint16_t temp;
    
    /* ����ʱ��1963���ı�ɨ�跽������IC�ı�ɨ�跽��
     * ����ʱ��1963�ı�ɨ�跽������IC���ı�ɨ�跽��
     */
    if (((lcddev.dir == 1) && (lcddev.id != 0x1963)) || ((lcddev.dir == 0) && (lcddev.id == 0x1963)))
    {
        switch (dir)   /* ����ת�� */
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
    
    /* ����ɨ�跽������0x36��0x3600�Ĵ���bit5~7λ��ֵ */
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
    
    /* 9341��7789��7796Ҫ����BGRλ */
    if ((lcddev.id == 0x9341) || (lcddev.id == 0x7789) || (lcddev.id == 0x7796))
    {
        regval |= 0x08;
    }
    
    lcd_write_reg(dirreg, regval);
    
    if (lcddev.id != 0x1963)                    /* 1963���������괦�� */
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)   /* ����X��Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   /* ����X��Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }
    
    /* ������ʾ���򣨿�������С */
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
 * @brief   ����
 * @param   x: ���X����
 * @param   y: ���Y����
 * @param   color: �����ɫ��32λ��ɫ���������LTDC��
 * @retval  ��
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    if (lcdltdc.pwidth != 0)        /* RGB�� */
    {
        ltdc_draw_point(x, y, color);
    }
    else
    {
        lcd_set_cursor(x, y);       /* ���ù��λ�� */
        lcd_write_ram_prepare();    /* ��ʼд��GRAM */
        LCD->LCD_RAM = color;
    }
}

/**
 * @brief   ����SSD1963��������
 * @param   pwm: ����ȼ�����Χ0~100����ֵԽ��Խ����
 * @retval  ��
 */
void lcd_ssd_backlight_set(uint8_t pwm)
{
    lcd_wr_regno(0xBE);         /* ����PWM��� */
    lcd_wr_data(0x05);          /* ����PWMƵ�� */
    lcd_wr_data(pwm * 2.55);    /* ����PWMռ�ձ� */
    lcd_wr_data(0x01);          /* ����C */
    lcd_wr_data(0xFF);          /* ����D */
    lcd_wr_data(0x00);          /* ����E */
    lcd_wr_data(0x00);          /* ����F */
}

/**
 * @brief   ����LCD��ʾ����
 * @param   dir: LCD��ʾ����
 *   @arg   0: ����
 *   @arg   1: ����
 * @retval  ��
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;
    
    if (lcdltdc.pwidth != 0)                                        /* RGB�� */
    {
        ltdc_display_dir(dir);
        lcddev.width = lcdltdc.width;
        lcddev.height = lcdltdc.height;
        return;
    }
    
    if (dir == 0)                                                   /* ���� */
    {
        lcddev.width = 240;
        lcddev.height = 320;
        
        if (lcddev.id == 0x5510)                                    /* 5510 */
        {
            lcddev.wramcmd = 0x2C00;                                /* ��ʼдGRAMָ�� */
            lcddev.setxcmd = 0x2A00;                                /* ����X����ָ�� */
            lcddev.setycmd = 0x2B00;                                /* ����Y����ָ�� */
            lcddev.width = 480;                                     /* LCD��� */
            lcddev.height = 800;                                    /* LCD�߶� */
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
    else                                                            /* ���� */
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
    
    lcd_scan_dir(DFT_SCAN_DIR);         /* ����LCDΪĬ��ɨ�跽�� */
}

/**
 * @brief   ���ô��ڣ���RGB����Ч��
 * @note    ���Զ����û������굽�������Ͻ�(sx,sy)
 * @param   sx    : ������ʼX����
 * @param   sy    : ������ʼY����
 * @param   width : ���ڿ�ȣ������0
 * @param   height: ���ڸ߶ȣ������0
 *  @note   ���ڴ�С = width * height
 * @retval  ��
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth;
    uint16_t theight;
    
    twidth = sx + width - 1;
    theight = sy + height - 1;
    
    if (lcdltdc.pwidth != 0)                        /* RGB�� */
    {
        return;
    }
    
    if ((lcddev.id == 0x1963) && (lcddev.dir != 1)) /* 1963�������� */
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
    else                                            /* 9341/5310/1963��������/7789 */
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
 * @brief   ����
 * @param   color: ��������ɫ
 * @retval  ��
 */
void lcd_clear(uint16_t color)
{
    uint32_t index;
    uint32_t totalpoint;
    
    if (lcdltdc.pwidth != 0)                        /* RGB�� */
    {
        ltdc_clear(color);
    }
    else
    {
        totalpoint = lcddev.width * lcddev.height;  /* �������������� */
        lcd_set_cursor(0x00, 0x0000);               /* ���ù��λ�� */
        lcd_write_ram_prepare();                    /* ��ʼд��GRAM */
        for (index=0; index<totalpoint; index++)
        {
            LCD->LCD_RAM = color;
        }
    }
}

/**
 * @brief   ��ָ����������䵥����ɫ
 * @param   sx    : ָ���������ʼX����
 * @param   sy    : ָ���������ʼY����
 * @param   ex    : ָ������Ľ���X����
 * @param   ey    : ָ������Ľ���Y����
 * @param   color : Ҫ������ɫ��32λ��ɫ���������LTDC��
 *  @note   ָ������Ĵ�С = (ex - sx + 1) * (ey - sy + 1)
 * @retval  ��
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i;
    uint16_t j;
    uint16_t xlen;
    
    
    if (lcdltdc.pwidth != 0)            /* RGB�� */
    {
        ltdc_fill(sx, sy, ex, ey, color);
    }
    else
    {
        xlen = ex - sx + 1;
        for (i=sy; i<=ey; i++)
        {
            lcd_set_cursor(sx, i);      /* ���ù��λ�� */
            lcd_write_ram_prepare();    /* ��ʼд��GRAM */
            for (j=0; j<xlen; j++)
            {
                LCD->LCD_RAM = color;
            }
        }
    }
}

/**
 * @brief   ��ָ�����������ָ����ɫ��
 * @param   sx    : ָ���������ʼX����
 * @param   sy    : ָ���������ʼY����
 * @param   ex    : ָ������Ľ���X����
 * @param   ey    : ָ������Ľ���Y����
 * @param   color : ָ����ɫ������׵�ַ
 *  @note   ָ������Ĵ�С = (ex - sx + 1) * (ey - sy + 1)
 * @retval  ��
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height;
    uint16_t width;
    uint16_t i;
    uint16_t j;
    
    if (lcdltdc.pwidth != 0)            /* RGB�� */
    {
        ltdc_color_fill(sx, sy, ex, ey, color);
    }
    else
    {
        width = ex - sx + 1;            /* ����ָ������Ŀ�� */
        height = ey - sy + 1;           /* ����ָ������ĸ߶� */
        for (i=0; i<height; i++)
        {
            lcd_set_cursor(sx, sy + i); /* ���ù��λ�� */
            lcd_write_ram_prepare();    /* ��ʼд��GRAM */
            for (j=0; j<width; j++)
            {
                LCD->LCD_RAM = color[i * width + j];
            }
        }
    }
}

/**
 * @brief   ����
 * @param   x1   : �ߵ���ʼX����
 * @param   y1   : �ߵ���ʼY����
 * @param   x2   : �ߵĽ���X����
 * @param   y2   : �ߵĽ���Y����
 * @param   color: �ߵ���ɫ
 * @retval  ��
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
    
    /* ������������ */
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    
    row = x1;
    col = y1;
    
    /* ����X�������� */
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
    
    /* ����Y�������� */
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
    
    /* ѡȡ�������������� */
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
 * @brief   ��ˮƽ��
 * @param   x    : �ߵ���ʼX����
 * @param   y    : �ߵ���ʼY����
 * @param   len  : �ߵĳ���
 * @param   color: �ߵ���ɫ
 * @retval  ��
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
 * @brief   ������
 * @param   x1   : �������Ͻ�X����
 * @param   y1   : �������Ͻ�Y����
 * @param   x2   : �������½�X����
 * @param   y2   : �������½�Y����
 * @param   color: ���ε���ɫ
 * @retval  ��
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief   ��Բ
 * @param   x0   : Բ�ĵ�X����
 * @param   y0   : Բ�ĵ�Y����
 * @param   r    : Բ�İ뾶
 * @param   color: Բ����ɫ
 * @retval  ��
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a;
    int b;
    int di;
    
    a = 0;
    b = r;
    di = 3 - (r << 1);  /* �ж��¸���λ�õı�־ */
    
    while (a <= b)      /* ʹ��Bresenham�㷨��Բ */
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
 * @brief   ��ʵ��Բ
 * @param   x    : Բ�ĵ�X����
 * @param   y    : Բ�ĵ�Y����
 * @param   r    : Բ�İ뾶
 * @param   color: Բ����ɫ
 * @retval  ��
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
 * @brief   ��ָ��λ����ʾһ���ַ�
 * @param   x    : ָ��λ�õ�X����
 * @param   y    : ָ��λ�õ�Y����
 * @param   chr  : Ҫ��ʾ���ַ�����Χ��' '~'~'
 * @param   size : ����
 *   @arg   12: 12*12 ASCII�ַ�
 *   @arg   16: 16*16 ASCII�ַ�
 *   @arg   24: 24*24 ASCII�ַ�
 *   @arg   32: 32*32 ASCII�ַ�
 * @param   mode : ��ʾģʽ
 *   @arg   0: �ǵ��ӷ�ʽ
 *   @arg   1: ���ӷ�ʽ
 * @param   color: �ַ�����ɫ
 * @retval  ��
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t csize;
    uint8_t *pfont;
    uint16_t y0;
    uint8_t t;
    uint8_t t1;
    uint8_t temp;
    
    csize = ((size >> 3) + (((size & 0x7) != 0) ? 1 : 0)) * (size >> 1);    /* ������ѡ�����Ӧһ���ַ���ռ���ֽ��� */
    chr -= ' ';                                                             /* ����ƫ�ƺ��ֵ����Ϊ�ֿ��Ǵӿո�ʼ�� */
    
    switch (size)
    {
        case 12:                                                            /* 12*12 ASCII�ַ� */
        {
            pfont = (uint8_t *)asc2_1206[chr];
            break;
        }
        case 16:                                                            /* 16*16 ASCII�ַ� */
        {
            pfont = (uint8_t *)asc2_1608[chr];
            break;
        }
        case 24:                                                            /* 24*24 ASCII�ַ� */
        {
            pfont = (uint8_t *)asc2_2412[chr];
            break;
        }
        case 32:                                                            /* 32*32 ASCII�ַ� */
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
        temp = pfont[t];                                                    /* ��ȡ�ַ��ĵ������� */
        for (t1=0; t1<8; t1++)                                              /* ����һ���ֽڵ�8��λ */
        {
            if ((temp & 0x80) != 0)                                         /* ��Ҫ��ʾ����Ч�� */
            {
                lcd_draw_point(x, y, color);                                /* ���ַ���ɫ��������� */
            }
            else if (mode == 0)                                             /* ����Ҫ��ʾ����Ч�� */
            {
                lcd_draw_point(x, y, g_back_color);                         /* ���Ʊ���ɫ */
            }
            
            temp <<= 1;                                                     /* ��λ����һ��λ */
            y++;
            if (y >= lcddev.height)                                         /* �ж�Y�����Ƿ񳬳���ʾ���� */
            {
                return;
            }
            if ((y - y0) == size)                                           /* ���л������ */
            {
                y = y0;                                                     /* Y���긴λ */
                x++;                                                        /* ��һ�� */
                if (x >= lcddev.width)                                      /* �ж�X�����Ƿ񳬳���ʾ���� */
                {
                    return;
                }
                break;
            }
        }
    }
}

/**
 * @brief   ƽ������
 * @param   m: ����
 * @param   n: ָ��
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
 * @brief   ��ʾlen������
 * @param   x    : ��ʼX����
 * @param   y    : ��ʼY����
 * @param   num  : ��ֵ����Χ��0~2^32
 * @param   len  : ��ʾ���ֵ�λ��
 * @param   size : ����
 *   @arg   12: 12*12 ASCII�ַ�
 *   @arg   16: 16*16 ASCII�ַ�
 *   @arg   24: 24*24 ASCII�ַ�
 *   @arg   32: 32*32 ASCII�ַ�
 * @param   color: ���ֵ���ɫ
 * @retval  ��
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t;
    uint8_t temp;
    uint8_t enshow = 0;
    
    for (t=0; t<len; t++)                                                   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                       /* ��ȡ��Ӧλ������ */
        if((enshow == 0) && (t < (len - 1)))                                /* û��ʹ����ʾ���һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size >> 1) * t, y, ' ', size, 0, color); /* ��ʾ�ո�ռλ */
                continue;                                                   /* ������һ��λ */
            }
            else
            {
                enshow = 1;                                                 /* ʹ����ʾ */
            }
        }
        
        lcd_show_char(x + (size >> 1) * t, y, temp + '0', size, 0, color);  /* ��ʾ�ַ� */
    }
}

/**
 * @brief   ��չ��ʾlen�����֣���ʾ��λ0��
 * @param   x    : ��ʼX����
 * @param   y    : ��ʼY����
 * @param   num  : ��ֵ����Χ��0~2^32
 * @param   len  : ��ʾ���ֵ�λ��
 * @param   size : ����
 *   @arg   12: 12*12 ASCII�ַ�
 *   @arg   16: 16*16 ASCII�ַ�
 *   @arg   24: 24*24 ASCII�ַ�
 *   @arg   32: 32*32 ASCII�ַ�
 * @param   mode : ��ʾģʽ
 *   @arg   0: �ǵ��ӷ�ʽ
 *   @arg   1: ���ӷ�ʽ
 * @param   color: ���ֵ���ɫ
 * @retval  ��
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t;
    uint8_t temp;
    uint8_t enshow = 0;
    
    for (t=0; t<len; t++)                                                                   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                                       /* ��ȡ��Ӧλ������ */
        if((enshow == 0) && (t < (len - 1)))                                                /* û��ʹ����ʾ���һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if ((mode & 0x80) != 0)                                                     /* ��λ��Ҫ���0 */
                {
                    lcd_show_char(x + (size >> 1) * t, y, '0', size, mode & 0x01, color);   /* ��ʾ0��ռλ */
                }
                else
                {
                    lcd_show_char(x + (size >> 1) * t, y, ' ', size, mode & 0x01, color);   /* ��ʾ�ո�ռλ */
                }
                continue;                                                                   /* ������һ��λ */
            }
            else
            {
                enshow = 1;                                                                 /* ʹ����ʾ */
            }
        }
        
        lcd_show_char(x + (size >> 1) * t, y, temp + '0', size, mode & 0x01, color);        /* ��ʾ�ַ� */
    }
}

/**
 * @brief   ��ʾ�ַ���
 * @param   x     : ��ʼX����
 * @param   y     : ��ʼY����
 * @param   width : ��ʾ������
 * @param   height: ��ʾ����߶�
 * @param   size  : ����
 *   @arg   12: 12*12 ASCII�ַ�
 *   @arg   16: 16*16 ASCII�ַ�
 *   @arg   24: 24*24 ASCII�ַ�
 *   @arg   32: 32*32 ASCII�ַ�
 * @param   *p    : �ַ���ָ��
 * @param   color : �ַ�������ɫ
 * @retval  ��
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0;
    
    x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' '))              /* �ж��Ƿ�Ϊ�Ƿ��ַ� */
    {
        if (x >= width)                             /* ���Խ�� */
        {
            x = x0;                                 /* ���� */
            y += size;
        }
        
        if (y >= height)                            /* �߶�Խ�� */
        {
            break;                                  /* �˳� */
        }
        
        lcd_show_char(x, y, *p, size, 0, color);    /* ��ʾһ���ַ� */
        x += (size >> 1);                           /* ASCII�ַ����Ϊ�߶ȵ�һ�� */
        p++;
    }
}

/**
 * @brief   ��ʼ��LCD
 * @param   ��
 * @retval  ��
 */
void lcd_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct;
//    FMC_NORSRAM_TimingTypeDef fmc_read_timing_struct = {0};
//    FMC_NORSRAM_TimingTypeDef fmc_write_timing_struct = {0};
    
//    /* ��ζ�ȡ�������ϵ�ʱIO��ƽ���������ĸ��� */
    lcddev.id = ltdc_panelid_read();
    lcddev.id = ltdc_panelid_read();

    if (lcddev.id != 0)
    {
        ltdc_init();
    }
    else
    {
//        /* ����ʱ�� */
//        rcc_periph_clk_init_struct.PeriphClockSelection |= RCC_PERIPHCLK_FMC;
//        rcc_periph_clk_init_struct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
//        HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct);
//        
//        /* ʹ��ʱ�� */
//        LCD_BL_GPIO_CLK_ENABLE();
//        
//        /* ����LCD BL���� */
//        gpio_init_struct.Pin = LCD_BL_GPIO_PIN;
//        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
//        gpio_init_struct.Pull = GPIO_PULLUP;
//        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//        HAL_GPIO_Init(LCD_BL_GPIO_PORT, &gpio_init_struct);
//        
//        /* ����FMC��ʱ�� */
//        fmc_read_timing_struct.AddressSetupTime = 15;
//        fmc_read_timing_struct.AddressHoldTime = 0;
//        fmc_read_timing_struct.DataSetupTime = 78;
//        fmc_read_timing_struct.AccessMode = FMC_ACCESS_MODE_A;
//        
//        /* ����FMCдʱ�� */
//        fmc_write_timing_struct.AddressSetupTime = 15;
//        fmc_write_timing_struct.AddressHoldTime = 0;
//        fmc_write_timing_struct.DataSetupTime = 15;
//        fmc_write_timing_struct.AccessMode = FMC_ACCESS_MODE_A;
//        
//        /* ����FMC */
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
//        /* ���Զ�ȡILI9341��ID */
//        lcd_wr_regno(0xD3);
//        lcddev.id = lcd_rd_data();                                                              /* ��Ч���� */
//        lcddev.id = lcd_rd_data();                                                              /* ��ȡ��0x00 */
//        lcddev.id = lcd_rd_data() << 8;                                                         /* ��ȡ��0x93 */
//        lcddev.id |= lcd_rd_data();                                                             /* ��ȡ��0x41 */
//        if (lcddev.id != 0x9341)                                                                /* ���Զ�ȡST7789��ID */
//        {
//            lcd_wr_regno(0x04);
//            lcddev.id = lcd_rd_data();                                                          /* ��Ч���� */
//            lcddev.id = lcd_rd_data();                                                          /* ��ȡ��0x85 */
//            lcddev.id = lcd_rd_data() << 8;                                                     /* ��ȡ��0x85 */
//            lcddev.id |= lcd_rd_data();                                                         /* ��ȡ��0x52 */
//            if (lcddev.id == 0x8552)                                                            /* ��ST7789��ID��Ϊ0x7789 */
//            {
//                lcddev.id = 0x7789;
//            }
//            if (lcddev.id != 0x7789)                                                            /* ���Զ�ȡNT35310��ID */
//            {
//                lcd_wr_regno(0xD4);
//                lcddev.id = lcd_rd_data();                                                      /* ��Ч���� */
//                lcddev.id = lcd_rd_data();                                                      /* ��ȡ��0x01 */
//                lcddev.id = lcd_rd_data() << 8;                                                 /* ��ȡ��0x53 */
//                lcddev.id |= lcd_rd_data();                                                     /* ��ȡ��0x10 */
//                if (lcddev.id != 0x5310)                                                        /* ���Զ�ȡST7796��ID */
//                {
//                    lcd_wr_regno(0XD3);
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id = lcd_rd_data();
//                    lcddev.id <<= 8;
//                    lcddev.id |= lcd_rd_data();
//                    if (lcddev.id != 0x7796)                                                    /* ���Զ�ȡNT35510��ID */
//                    {
//                        /* ������Կ�������ṩ�� */
//                        lcd_write_reg(0xF000, 0x0055);
//                        lcd_write_reg(0xF001, 0x00AA);
//                        lcd_write_reg(0xF002, 0x0052);
//                        lcd_write_reg(0xF003, 0x0008);
//                        lcd_write_reg(0xF004, 0x0001);
//                        
//                        lcd_wr_regno(0xC500);
//                        lcddev.id = lcd_rd_data() << 8;                                         /* ��ȡ��0x80 */
//                        lcd_wr_regno(0xC501);
//                        lcddev.id |= lcd_rd_data();                                             /* ��ȡ��0x00 */
//                        delay_ms(5);                                                            /* ��Ϊ��NT5510��ָ��ǡ��ΪSSD1963�������λָ�� */
//                        if (lcddev.id != 0x5510)                                                /* ���Զ�ȡILI9806��ID */
//                        {
//                            lcd_wr_regno(0XD3);
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id = lcd_rd_data();
//                            lcddev.id <<= 8;
//                            lcddev.id |= lcd_rd_data();
//                            if (lcddev.id != 0x9806)                                            /* ���Զ�ȡSSD1963��ID */
//                            {
//                                lcd_wr_regno(0xA1);
//                                lcddev.id = lcd_rd_data();
//                                lcddev.id = lcd_rd_data() << 8;                                 /* ��ȡ��0x57 */
//                                lcddev.id |= lcd_rd_data();                                     /* ��ȡ��0x61 */
//                                if (lcddev.id == 0x5761)                                        /* ��SSD1963��ID��Ϊ0x1963 */
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
//            lcd_ex_st7789_reginit();                                                            /* ִ��ST7789��ʼ�� */
//        }
//        else if (lcddev.id == 0x9341)
//        {
//            lcd_ex_ili9341_reginit();                                                           /* ִ��ILI9341��ʼ�� */
//        }
//        else if (lcddev.id == 0x5310)
//        {
//            lcd_ex_nt35310_reginit();                                                           /* ִ��NT35310��ʼ�� */
//        }
//        else if (lcddev.id == 0x7796)
//        {
//            lcd_ex_st7796_reginit();                                                            /* ִ��ST7796��ʼ�� */
//        }
//        else if (lcddev.id == 0x5510)
//        {
//            lcd_ex_nt35510_reginit();                                                           /* ִ��NT35510��ʼ�� */
//        }
//        else if (lcddev.id == 0x9806)
//        {
//            lcd_ex_ili9806_reginit();                                                           /* ִ��ILI9806��ʼ�� */
//        }
//        else if (lcddev.id == 0x1963)
//        {
//            lcd_ex_ssd1963_reginit();                                                           /* ִ��SSD1963��ʼ�� */
//            lcd_ssd_backlight_set(100);                                                         /* ��������Ϊ���� */
//        }
//        
//        /* ��ʼ����ɺ����� */
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
    
    lcd_display_dir(1);                                                                         /* Ĭ������Ϊ���� */
    LCD_BL(1);                                                                                  /* �������� */
    lcd_clear(WHITE);                                                                           /* ���� */
		printf("%s\r\n",__FUNCTION__);
}

///**
// * @brief   HAL��SRAM��ʼ��MSP����
// * @param   ��
// * @retval  ��
// */
//void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    
//    if (hsram->Instance == FMC_NORSRAM_DEVICE)
//    {
//        /* ʹ��ʱ�� */
//        __HAL_RCC_FMC_CLK_ENABLE();
//        __HAL_RCC_GPIOD_CLK_ENABLE();
//        __HAL_RCC_GPIOE_CLK_ENABLE();
//        LCD_WR_GPIO_CLK_ENABLE();
//        LCD_RD_GPIO_CLK_ENABLE();
//        LCD_CS_GPIO_CLK_ENABLE();
//        LCD_RS_GPIO_CLK_ENABLE();
//        
//        /* ����FMC���� */
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
