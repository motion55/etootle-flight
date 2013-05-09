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

// bsp/time/time.c
// 2012-11-3 12:01:00
// js200300953

#include "time.h"
#include "stm32f10x.h"

static struct 
{
    volatile uint32_t msPeriod;    // 整周期的时间，ms。
    uint32_t ticksPerUs;  // 每us等于的tick数。
    uint32_t ticksPerMs;  // 每ms等于的tick数。
    uint32_t msPerPeriod; // 每周期的ms数。
}time;

// 初始化时间。
void time_init(void)
{
    time.msPeriod = 0;
    time.ticksPerUs = SystemCoreClock / 1e6;
    time.ticksPerMs = SystemCoreClock / 1e3;
    time.msPerPeriod = 10;
    //
    SysTick_Config(SystemCoreClock/(1000/time.msPerPeriod));
}

// SysTick中断。
void SysTick_Handler(void)
{
    time.msPeriod += time.msPerPeriod;
}

// 获取当前时间，us。
uint64_t time_nowUs(void)
{
    return time.msPeriod * (uint64_t)1000 + (SysTick->LOAD - SysTick->VAL) / time.ticksPerUs;
}

// 获取当前时间，ms。
uint32_t time_nowMs(void)
{
    return time.msPeriod + (SysTick->LOAD - SysTick->VAL) / time.ticksPerMs;
}

// 延时delay us，delay>=4时才准确。
void time_waitUs(uint32_t delay)
{
    uint64_t target = time_nowUs() + delay - 2;
    while(time_nowUs() <= target)
        ; // 空循环。
}

// 延时delay ms。
void time_waitMs(uint32_t delay)
{
    time_waitUs(delay * 1000);
}
