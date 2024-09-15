//#include "MY_USART.h"
//#include "delay.h"
//#include "stm32h7xx_it.h"
//#define u8 unsigned char


//u8 recvData = 0;
//u8 recvStat = COM_STOP_BIT;

//// 用以模拟串口延时的定时器初始化
//void TIM2_Int_Init(void) {
//    TIM_HandleTypeDef TimHandle;
//    
//    __HAL_RCC_TIM2_CLK_ENABLE();
//    
//    TimHandle.Instance = TIM2;
//    TimHandle.Init.Period = 65535 - 1;
//    TimHandle.Init.Prescaler = 240 - 1;  // 240 MHz clock, 1 MHz timer frequency
//    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
//    
//    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
//        // Initialization Error
//        Error_Handler();
//    }
//    
//    if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK) {
//        // Starting Error
//        Error_Handler();
//    }
//}

//// 用以模拟io串口数据接收计时的定时器初始化
//void TIM4_Int_Init(void) {
//    TIM_HandleTypeDef TimHandle;
//    TIM_ClockConfigTypeDef sClockSourceConfig;
//    TIM_MasterConfigTypeDef sMasterConfig;
//    NVIC_InitTypeDef NVIC_InitStructure;
//    
//    __HAL_RCC_TIM4_CLK_ENABLE();
//    
//    TimHandle.Instance = TIM4;
//    TimHandle.Init.Period = IO_USART_SENDDELAY_TIME - 1;
//    TimHandle.Init.Prescaler = 240 - 1;  // 240 MHz clock, 1 MHz timer frequency
//    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
//    
//    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
//        // Initialization Error
//        Error_Handler();
//    }
//    
//    HAL_TIM_Base_Start_IT(&TimHandle);
//    
//    HAL_NVIC_SetPriority(TIM4_IRQn, 3, 1);
//    HAL_NVIC_EnableIRQ(TIM4_IRQn);
//}

//// 模拟串口1初始化
//void iouart1_init(void) {
//    GPIO_InitTypeDef GPIO_InitStruct;
//    
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    
//    // SoftWare Serial TXD
//    GPIO_InitStruct.Pin = GPIO_PIN_8;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
//    
//    // SoftWare Serial RXD
//    GPIO_InitStruct.Pin = GPIO_PIN_9;
//    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//    
//    // EXTI interrupt init
//    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
//    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//    
//    TIM2_Int_Init();
//    TIM4_Int_Init();
//}

//void iouart1_TXD(uint8_t option) {
//    if (option == 1) {
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
//    } else {
//        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
//    }
//}

//uint8_t iouart1_RXD(void) {
//    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
//}

//// 延时函数 (微秒)
//void iouart1_delayUs(volatile uint32_t nTime) {
//    uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);
//    
//    while ((__HAL_TIM_GET_COUNTER(&htim2) - start) < nTime);
//}

//// 模拟串口发送一字节数据
//void iouart1_SendByte(uint8_t datatoSend) {
//    uint8_t i, tmp;
//    
//    // 开始位
//    iouart1_TXD(0);
//    iouart1_delayUs(IO_USART_SENDDELAY_TIME);
//    
//    for (i = 0; i < 8; i++) {
//        tmp = (datatoSend >> i) & 0x01;
//        
//        if (tmp == 0) {
//            iouart1_TXD(0);
//        } else {
//            iouart1_TXD(1);
//        }
//        iouart1_delayUs(IO_USART_SENDDELAY_TIME);
//    }
//    
//    // 结束位
//    iouart1_TXD(1);
//    iouart1_delayUs(IO_USART_SENDDELAY_TIME);
//}

//// EXTI9_5中断处理函数
//void EXTI9_5_IRQHandler(void) {
//    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET) {
//        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
//        
//        if (iouart1_RXD() == 0) {
//            if (recvStat == COM_STOP_BIT) {
//                recvStat = COM_START_BIT;
//                iouart1_delayUs(50);
//                HAL_TIM_Base_Start_IT(&htim4);
//            }
//        }
//    }
//}

//// TIM4中断处理函数
//void TIM4_IRQHandler(void) {
//    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET) {
//        __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
//        
//        recvStat++;
//        if (recvStat == COM_STOP_BIT) {
//            HAL_TIM_Base_Stop_IT(&htim4);
//            // 到这里就接收到完整的一个字节数据
//            // 处理接收到的数据 recvData
//            return;
//        }
//        if (iouart1_RXD()) {
//            recvData |= (1 << (recvStat - 1));
//        } else {
//            recvData &= ~(1 << (recvStat - 1));
//        }
//    }
//}
