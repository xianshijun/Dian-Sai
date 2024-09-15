/**
 ****************************************************************************************************
 * @file        lcd.h
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

#ifndef __LCD_H
#define __LCD_H

#include "delay.h"

/* LCD�������Ŷ���
 * LCD D0~D15����ֱ���ں���lcd_init()�ж���
 */
#define LCD_WR_GPIO_PORT            GPIOD
#define LCD_WR_GPIO_PIN             GPIO_PIN_5
#define LCD_WR_GPIO_AF              GPIO_AF12_FMC
#define LCD_WR_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_RD_GPIO_PORT            GPIOD
#define LCD_RD_GPIO_PIN             GPIO_PIN_4
#define LCD_RD_GPIO_AF              GPIO_AF12_FMC
#define LCD_RD_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_BL_GPIO_PORT            GPIOB
#define LCD_BL_GPIO_PIN             GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

#define LCD_CS_GPIO_PORT            GPIOD
#define LCD_CS_GPIO_PIN             GPIO_PIN_7
#define LCD_CS_GPIO_AF              GPIO_AF12_FMC
#define LCD_CS_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_RS_GPIO_PORT            GPIOD
#define LCD_RS_GPIO_PIN             GPIO_PIN_13
#define LCD_RS_GPIO_AF              GPIO_AF12_FMC
#define LCD_RS_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

/* LCD���Ų��� */
#define LCD_BL(x)                   do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET);   \
                                    } while (0)

/* FMC��ز�������
 * Ĭ��ͨ��FMC�Ĵ洢��1������LCD���洢��1��4��Ƭѡ��FMC_NE1~4����ӦFMC�洢��1��4�������
 */
#define LCD_FMC_NEX                1       /* ʹ��FMC_NE1����LCD_CS����Χ��1~4 */
#define LCD_FMC_AX                 18      /* ʹ��FMC_A18����LCD_RS����Χ��0~25 */

/* LCD��ַ�ṹ�� */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;

/* LCD_BASE����ϸ���㷽����
 * һ��ʹ��FMC�Ĵ洢��1������TFTLCDҺ������MCU�������洢��1��ַ��Χ�ܴ�СΪ256MB�����ֳ�4����飺
 * �洢��1�����1��FMC_NE1����ַ��Χ��0x60000000~0x63FFFFFF
 * �洢��1�����2��FMC_NE2����ַ��Χ��0x64000000~0x67FFFFFF
 * �洢��1�����3��FMC_NE3����ַ��Χ��0x68000000~0x6BFFFFFF
 * �洢��1�����4��FMC_NE4����ַ��Χ��0x6C000000~0x6FFFFFFF
 *
 * ��Ҫ����Ӳ�����ӷ�ʽѡ����ʵ�Ƭѡ������LCD_CS���͵�ַ�ߣ�����LCD_RS��
 * ����ԭ�� ������ H750������ʹ��FMC_NE1����LCD_CS��FMC_A18����LCD_RS��16λ�����ߣ����㷽�����£�
 * ����FMC_NE1�Ļ���ַΪ: 0x60000000��FMC_NEx��x=1~4���Ļ�ַΪ: 0x60000000 + (0x4000000 * (x - 1))
 * FMC_A18��Ӧ��ֵַ��2^18 * 2 = 0x80000��FMC_Ay��y=0~25���ĵ�ַΪ��2^y * 2
 *
 * LCD->LCD_REG����ӦLCD_RS = 0��LCD�Ĵ�������LCD->LCD_RAM����ӦLCD_RS = 1��LCD���ݣ�
 * LCD->LCD_RAM�ĵ�ַΪ��0x60000000 + 0x80000 = 0x60080000
 * LCD->LCD_REG�ĵ�ַΪ��LCD->LCD_RAM֮��������ַ
 * ����ʹ���˽ṹ�����LCD_REG��LCD_RAM��LCD_REG��ǰ��LCD_RAM�ں󣬾�Ϊ16λ���ݿ�ȣ�
 * ��˽ṹ��Ļ���ַ��LCD_BASE�� = LCD_RAM - sizeof(LCD_REG) = 0x6007FFFE
 *
 * ����ͨ�õļ��㹫ʽΪ��LCD_CS��Ƭѡ��FMC_NEx��x=1~4������LCD_RS�ӵ�ַ��FMC_Ay��y=0~25������
 * LCD_BASE = (0x60000000 + (0x4000000 * (x - 1))) | (2^y * 2 - 2)
 * ��Ч�ڣ�ʹ����λ������
 * LCD_BASE = (0x60000000 + (0x4000000 * (x - 1))) | ((1 << y) * 2 - 2)
 */
#define LCD_BASE                    (uint32_t)((0x60000000 + (0x4000000 * (LCD_FMC_NEX - 1))) | (((1 << LCD_FMC_AX) * 2) - 2))
#define LCD                         ((LCD_TypeDef *)LCD_BASE)

/* LCD��Ҫ������ */
typedef struct
{
    uint16_t width;                         /* LCD��� */
    uint16_t height;                        /* LCD�߶� */
    uint16_t id;                            /* LCD ID */
    uint8_t dir;                            /* ��������־λ��0��������1������ */
    uint16_t wramcmd;                       /* ��ʼдGRAMָ�� */
    uint16_t setxcmd;                       /* ����X����ָ�� */
    uint16_t setycmd;                       /* ����Y����ָ�� */
} _lcd_dev;

/* LCD�������� */
extern _lcd_dev lcddev; /* ���ڹ���LCD��Ҫ�Ĳ��� */

/* LCDɨ�跽���� */
#define L2R_U2D                     0       /* �����ң����ϵ��� */
#define L2R_D2U                     1       /* �����ң����µ��� */
#define R2L_U2D                     2       /* ���ҵ��󣬴��ϵ��� */
#define R2L_D2U                     3       /* ���ҵ��󣬴��µ��� */
#define U2D_L2R                     4       /* ���ϵ��£������� */
#define U2D_R2L                     5       /* ���ϵ��£����ҵ��� */
#define D2U_L2R                     6       /* ���µ��ϣ������� */
#define D2U_R2L                     7       /* ���µ��ϣ����ҵ��� */
#define DFT_SCAN_DIR                L2R_U2D /* Ĭ�ϵ�ɨ�跽�� */

/* ��ɫ���� */
#define WHITE                       0xFFFF  /* ��ɫ */
#define BLACK                       0x0000  /* ��ɫ */
#define RED                         0xF800  /* ��ɫ */
#define GREEN                       0x07E0  /* ��ɫ */
#define BLUE                        0x001F  /* ��ɫ */
#define MAGENTA                     0xF81F  /* Ʒ��ɫ */
#define YELLOW                      0xFFE0  /* ��ɫ */
#define CYAN                        0x07FF  /* ��ɫ */
#define BROWN                       0xBC40  /* ��ɫ */
#define BRRED                       0xFC07  /* �غ�ɫ */
#define GRAY                        0x8430  /* ��ɫ */
#define DARKBLUE                    0x01CF  /* ����ɫ */
#define LIGHTBLUE                   0x7D7C  /* ǳ��ɫ */
#define GRAYBLUE                    0x5458  /* ����ɫ */
#define LIGHTGREEN                  0x841F  /* ǳ��ɫ */
#define LGRAY                       0xC618  /* ǳ��ɫ */
#define LGRAYBLUE                   0xA651  /* ǳ����ɫ */
#define LBBLUE                      0x2B12  /* ǳ����ɫ */

/* SSD1963������ö��壨һ�������޸ģ� */
#define SSD_HOR_RESOLUTION          800     /* LCDˮƽ�ֱ��� */
#define SSD_VER_RESOLUTION          480     /* LCD��ֱ�ֱ��� */
#define SSD_HOR_PULSE_WIDTH         1       /* ˮƽ���� */
#define SSD_HOR_BACK_PORCH          46      /* ˮƽǰ�� */
#define SSD_HOR_FRONT_PORCH         210     /* ˮƽ���� */
#define SSD_VER_PULSE_WIDTH         1       /* ��ֱ���� */
#define SSD_VER_BACK_PORCH          23      /* ��ֱǰ�� */
#define SSD_VER_FRONT_PORCH         22      /* ��ֱǰ�� */
#define SSD_HT                      (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS                     (SSD_HOR_BACK_PORCH)
#define SSD_VT                      (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS                     (SSD_VER_BACK_PORCH)

/* ����LCDʱ�ı���ɫ */
extern uint32_t g_back_color;               /* Ĭ��Ϊ��ɫ */

/* �������� */
void lcd_wr_data(volatile uint16_t data);                                                                               /* LCDд���� */
void lcd_wr_regno(volatile uint16_t regno);                                                                             /* LCDд�Ĵ�����Ż��ַ */
void lcd_write_reg(uint16_t regno, uint16_t data);                                                                      /* LCDд�Ĵ��� */
void lcd_init(void);                                                                                                    /* ��ʼ��LCD */
void lcd_display_on(void);                                                                                              /* LCD������ʾ */
void lcd_display_off(void);                                                                                             /* LCD�ر���ʾ */
void lcd_scan_dir(uint8_t dir);                                                                                         /* ����LCD���Զ�ɨ�跽�򣨶�RGB����Ч�� */
void lcd_display_dir(uint8_t dir);                                                                                      /* ����LCD��ʾ���� */
void lcd_ssd_backlight_set(uint8_t pwm);                                                                                /* ����SSD1963�������� */
void lcd_write_ram_prepare(void);                                                                                       /* ׼��дGRAM */
void lcd_set_cursor(uint16_t x, uint16_t y);                                                                            /* ���ù��λ�ã���RGB����Ч�� */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                                         /* ���ô��ڣ���RGB����Ч�� */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);                                                            /* ���� */
uint32_t lcd_read_point(uint16_t x, uint16_t y);                                                                        /* ��ȡ��ĳ�����ɫֵ */
void lcd_clear(uint16_t color);                                                                                         /* ���� */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);                                      /* ��ָ����������䵥����ɫ */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);                               /* ��ָ�����������ָ����ɫ�� */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                                 /* ���� */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                                              /* ��ˮƽ�� */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                            /* ������ */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                                              /* ��Բ */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                                               /* ��ʵ��Բ */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);                       /* ��ָ��λ����ʾһ���ַ� */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);                     /* ��ʾlen������ */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);      /* ��չ��ʾlen�����֣���ʾ��λ0�� */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);   /* ��ʾ�ַ��� */


#endif
