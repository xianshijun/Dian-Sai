# 关于正点原子H750北极星核心板的外部QSPIfalsh启动的BootLoader的上手指南

## 一.前言

因为像 STM32H750、730 以及 7B0 这类单片机片内的 flash 空
间很小，通常只有 128KB，很多时候无法满足使用，所以需要将程序
下载到片外扩展的 flash 中。（虽然小，但是性价比高一些，外部flash较便宜）
现在目标就是将代码下载到外部芯片中（如W25Q64）,但是一般的外部芯片还不行，因为读取的速度太慢了，影响性能发挥。
所以要使用QSPI（注：单片机和flash芯片都要相互支持才行）来进行，这样才能保证好的速率。

## 二.硬件平台讲解

### 北极星开发板图：

### <img src="https://gitee.com/xianshijun/typora/raw/master/master/202409151108233.jpg" alt="25b398ff45755b24760abfe4ef0b66d3" style="zoom: 25%;" />

### 核心板图：

我们实验室买了很多这样的板子，但是没人学，不忍心资源的浪费好吧。我就在此基础上绘制了拓展板，这样也可以用来打电赛了。

### <img src="https://gitee.com/xianshijun/typora/raw/master/master/202409151108101.jpg" alt="8ef69dd65343be44da399b8d9ab3722b" style="zoom: 25%;" />

### 电赛拓展板图：

V1.0拓展板设计说明：

PCB板使用两层板设计（嘉立创两层板可以选颜色，黑色沉金真的很好看好吧）
拓展板是将核心板的引脚引出，添加了按键和ADC、DAC的SMA接口引出。

还引出了串口1和串口4和STlink牛角管方便调试。
使用了HT7333芯片，将5V转为3.3V，引出参考电压引脚使用跳线帽选择参考电压3.3V or 0V

<img src="https://gitee.com/xianshijun/typora/raw/master/master/202409151110304.jpg" alt="f7a0f9a1e4ed14ac21ba35013627c915" style="zoom:33%;" />

<img src="https://gitee.com/xianshijun/typora/raw/master/master/202409151111910.jpg" alt="e9b7d487f8ec6795b0bc0739aac7d07b" style="zoom: 25%;" />





## 三.原理讲解

### BootLoader思路讲解

即内部代码控制指针运行外部芯片中已经下载的代码

所以有两份代码工程，一个下载到内部的负责初始化QSPI然后跳转。另外一个负责执行你需要的程序

![image-20240915162000755](https://gitee.com/xianshijun/typora/raw/master/master/202409151620807.png)

#### 核心板原理图解释：

通过PCB原理图查看W25Q64的引脚配置

PB2----->CLK
PB6----->NCS
PF6----->IO3
PF7----->IO2

PF8----->IO0
PF9----->IO1

在BOOT文件里修改对应好自己的板子



![image-20240915112139351](https://gitee.com/xianshijun/typora/raw/master/master/202409151121373.png)







## 四.工程文件

Boot启动文件是源自反客（一般情况下都一次烧录后就不会再修改了）

App文件我已经移植成了cubemx的工程，集合了LTDC、触控、DMA2D、串口等，方便实验室的成员直接使用正点原子的7寸RGB屏幕。

仓库链接：[Dian-Sai/ at main · xianshijun/Dian-Sai (github.com)](https://github.com/xianshijun/Dian-Sai/tree/main)



## 五.快速上手工程文件



### 内部Flash下载Boot引导程序

就像一般程序一样直接下载进去


![image-20240915161656127](https://gitee.com/xianshijun/typora/raw/master/master/202409151616198.png)



![image-20240915161737039](https://gitee.com/xianshijun/typora/raw/master/master/202409151617070.png)



![image-20240915161809674](https://gitee.com/xianshijun/typora/raw/master/master/202409151618707.png)

**确认完这些配置后下载代码到芯片中**，然后可以在串口一中看到情况

![image-20240915162726143](https://gitee.com/xianshijun/typora/raw/master/master/202409151627176.png)

### 添加外部下载算法

ATK-DNH750_QSPI_W25Q64JV.FLM是正点原子的核心板提供的外部下载算法。需要添加到keil里面才能选择。

1.找到桌面keilMDK的快捷方式，右键后选择“打开文件所在位置”

2.返回一级后找到ARM/Flash 将下载算法粘贴到此目录下（注：下载算法链接[xianshijun/Dian-Sai: 记录我在实验室里写的重要代码 (github.com)](https://github.com/xianshijun/Dian-Sai/tree/main)

![](https://gitee.com/xianshijun/typora/raw/master/master/202409151605817.png)



### 下载代码到外部Flash

![image-20240915164658485](https://gitee.com/xianshijun/typora/raw/master/master/202409151646516.png)





![image-20240915164725748](https://gitee.com/xianshijun/typora/raw/master/master/202409151647778.png)





![image-20240915164809342](https://gitee.com/xianshijun/typora/raw/master/master/202409151648373.png)





注意：此处**只**留下一个外部下载算法

![image-20240915164905142](https://gitee.com/xianshijun/typora/raw/master/master/202409151649175.png)



检查一下，看看此处是否添加的这个代码，要求必须有这行代码

![image-20240915165213813](https://gitee.com/xianshijun/typora/raw/master/master/202409151652848.png)

最后全编译然后下载，就能正常运行了。

## 六.后言

此教程为电赛快速上手教程，UP断断续续地折腾了一个月。希望看客们留下关注和点赞。
