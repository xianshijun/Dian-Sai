/*
*
*
*
*
*
*
*
*
*
*/
#ifndef __MINOR_H
#define __MINOR_H
///////////////////////Ԥ����/////////////////////////////
#include "delay.h"
#include "lcd.h"
#include "MY_ADC.h"
///////////////////�ṹ���Լ�ȫ�ֱ�������/////////////////
typedef struct //����ṹ��
{
	uint16_t x1;//��ʼ����
	uint16_t y1;
	uint16_t x2;//��ֹ����
	uint16_t y2;
}coord;

extern const coord Screen;//��Ļ��Χ


//////////////////////////////////////////////////////////
void Waveform_rendering(uint16_t Arr[]);
void Oscilloscope_Window(void);
void Minor_Function(void);



#endif
