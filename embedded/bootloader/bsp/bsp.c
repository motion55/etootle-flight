//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ========圆点博士微型四轴飞行器配套程序声明========
// 
// 圆点博士微型四轴飞行器配套程序包括上位机程序、下位机Bootloader和
//     下位机App，及它们的源代码，以下总称“程序”。
// 程序由js200300953编写。
// 程序仅为使用者提供参考，js200300953不对程序提供任何明示或暗含的担保，
//     不对在使用该程序中出现的意外或者损失负责，
//     也不对因使用该程序而引起的第三方索赔负责。
// 使用者可以以学习为目的修改和使用该程序，请勿以商业的目的使用该程序。
// 修改该程序时，必须保留原版权声明，并且不能修改原版权声明。
// 
// 更多资料见：
//     http://blog.sina.com.cn/js200300953
//     http://www.etootle.com/
//     http://www.amobbs.com/thread-5504090-1-1.html
//     圆点博士微型四轴飞行器QQ群：276721324

// bsp/bsp.c
// 2013-1-7 13:13:47
// js200300953

#include "stm32f10x.h"
#include "bsp.h"
#include "led.h"
#include "time.h"
#include "rf.h"
#include "uart.h"

void bsp_init(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,(uint32_t)0x0000);
    
    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_DMA1,ENABLE);
    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_TIM2 |
        RCC_APB1Periph_I2C1 |
        RCC_APB1Periph_USB,ENABLE);
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA    |
        RCC_APB2Periph_GPIOB    |
        RCC_APB2Periph_AFIO     |
        RCC_APB2Periph_USART1   |
        RCC_APB2Periph_SPI1     |
        RCC_APB2Periph_ADC1,ENABLE);
    //
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    __enable_irq();
    //
    time_init();
    time_waitMs(100);
    //
    led_init();
    //
    int32_t rs = rf_init();
    if(rs != 0)
        while(1);
    rf_startReceive();
    uart1_init(115200);
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while(1)
    {
    }
}
