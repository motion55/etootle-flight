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

// bsp/hal.c
// 2012-9-2 9:39:32
// js200300953

#include "stm32f10x.h"
#include "rf_hal.h"

void    rf_hal_init(void);
void    rf_hal_ceHigh(void);
void    rf_hal_ceLow(void);
void    rf_hal_spiBegin(void);
void    rf_hal_spiEnd(void);
uint8_t rf_hal_spiSwap(uint8_t val);

void rf_hal_init(void)
{
    //
    // 初始化针脚。
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // A4 => CSN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
    //
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; // A5 => CLK , A7 => MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // B2 => CE
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
    //
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8; // A6 => MISO , A8 => IRQ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //
    // 初始化SPI。
    SPI_I2S_DeInit(SPI1);
    SPI_Cmd(SPI1,DISABLE);
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1,&SPI_InitStructure);
    SPI_Cmd(SPI1,ENABLE);
}

void rf_hal_ceHigh(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_2);
}

void rf_hal_ceLow(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
}

int32_t rf_hal_irqIsHigh(void)
{
    return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
}

void rf_hal_spiBegin(void)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

void rf_hal_spiEnd(void)
{
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

/* 通过SPI发送并返回一个字节。*/
uint8_t rf_hal_spiSwap(uint8_t val)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        // 空循环。
    }
    SPI_I2S_SendData(SPI1,val);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        // 空循环。
    }
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}
