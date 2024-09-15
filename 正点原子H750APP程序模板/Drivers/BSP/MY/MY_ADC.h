#ifndef __MYADC_H
#define __MYADC_H
///////////////////////////////////////////////////////////////////
#include "main.h"
#include "stm32h7xx_it.h"
#include "math.h"
#include "arm_math.h"

///////////////////////////////////////////////////////////////////
#define ADC_Quantity 8192
#define FFT_Quantity 8192
#define ADC_Resolution_Ratio 65535


#define fft_psc 1
#define fft_arr 80
//////////////////////////////////用来调用数据分析后的各个参数//////////////////////////////////////////////////
typedef  struct
{
    int     N;						//采样点数
    double  fs;						//采样率
    double  dt;						//采样时间间隔 == 1/fs
    double  fm;						//信号频率
    double  VPP;					//峰峰值
    double  mean_value;		//平均值
    float   DC;						//直流分量
    float   range;				//幅度
    float   phase;				//相位
    float   THD;					//总谐波失真度
		char 		Str1[20];			//字符串1
}F_F_T;
extern F_F_T FFT;
///////////////////////////////////*用来做FFT的复数数组*///////////////////////////////////
typedef struct  compx 
{
	float32_t real, imag;//实部 虚部
}COMPX;   
extern COMPX FFT_ARR[FFT_Quantity];
extern float32_t Be_fm_Arr[1024];//储存扫频频谱图

///////////////////////////////////*用来分析FFT后的图像*///////////////////////////////////
typedef struct 
{
		uint16_t index;//峰值下标
		double peak;
		double fs;
}_PEAK;
extern _PEAK Peak_ARR[20];//分析前20个峰值


///////////////////////////////////*用来存放ADC采集到的值*//////////////////////////////////
extern uint16_t ADC_Val_Arr[ADC_Quantity];

///////////////////////////////////*无限点FFT辅助函数*///////////////////////////////////
void InitTableFFT(uint32_t n);


///////////////////////////////////*用来做FFT*///////////////////////////////////
void cfft(struct compx *_ptr, uint32_t FFT_N );


///////////////////////////////////*用来实施操作*///////////////////////////////////
void FFT_refresh(void);




///////////////////////////////////*用来计算THD值*///////////////////////////////////
float THD(float fft_outputbuf[]);      



#endif
