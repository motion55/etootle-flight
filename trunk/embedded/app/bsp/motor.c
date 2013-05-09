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

// bsp/motor.c
// 2013-1-8 11:43:58
// js200300953

#include "stm32f10x.h"
#include "motor.h"

#define MOTOR_ARR 72*100        // CCR重装值，用于设定周期，72M/(72*100)==10k。
#define MOTOR_CCR_MIN 0         // 最小油门对应的CCR值。
#define MOTOR_CCR_MAX MOTOR_ARR // 最大油门对应的CCR值。

#define ESC_PSC     (1-1) // 72M分频，1分频。

void motor_init(void)
{
    // 设置GPIO功能。
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //
    // 复位定时器。
    TIM_DeInit(TIM2);
    //
    // 配置计时器。
    TIM_TimeBaseInitTypeDef param;
    TIM_TimeBaseStructInit(&param);
    param.TIM_Prescaler = ESC_PSC;
    param.TIM_Period = MOTOR_ARR;
    param.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&param);
    //
    // 配置PWM。
    TIM_OCInitTypeDef pwmParam;
    TIM_OCStructInit(&pwmParam);
    pwmParam.TIM_OCMode = TIM_OCMode_PWM1;
    pwmParam.TIM_OutputState = TIM_OutputState_Enable;
    pwmParam.TIM_Pulse = MOTOR_CCR_MIN;
    pwmParam.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2,&pwmParam);
    TIM_OC2Init(TIM2,&pwmParam);
    TIM_OC3Init(TIM2,&pwmParam);
    TIM_OC4Init(TIM2,&pwmParam);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    //
    // 启动计时器。
    TIM_Cmd(TIM2,ENABLE);
}

void motor_setForce(const float force[4])
{
    uint16_t f[4];
    for(int32_t i=0;i<4;i++)
    {
        if(force[i] < MOTOR_FORCE_MIN)
            f[i] = MOTOR_CCR_MIN;
        else if(force[i] > MOTOR_FORCE_MAX)
            f[i] = MOTOR_CCR_MAX;
        else
            f[i] = (uint16_t)(force[i] * (MOTOR_CCR_MAX - MOTOR_CCR_MIN)) + MOTOR_CCR_MIN;
    }
    //
    TIM_SetCompare1(TIM2,f[2]);
    TIM_SetCompare2(TIM2,f[3]);
    TIM_SetCompare3(TIM2,f[0]);
    TIM_SetCompare4(TIM2,f[1]);
}
