/************************************************
圆点博士微型四轴飞行器版权和声明：
圆点博士所提供的下列代码仅为使用者提供参考，而圆点博士不对该代码提供任何明示或暗含的担保。
圆点博士不对在使用该代码中可能出现的意外或者损失负责，也不对因使用该代码而引起的第三方索赔负责。
该代码可能会被随时变更，相关信息的更新恕不另行通知。
该代码仅以学习的形式发布，并受版权保护, 未经书面授权, 请勿以商业的目的使用或者修改该代码。
圆点博士对该代码保留最终解释权。
圆点博士微型四轴飞行器QQ群：276721324

Dr.R&D DISCLAIMER
THIS CODE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY, 
AND WITH NO CLAIM AS TO ITS SUITABILITY FOR ANY PURPOSE. 
WE ARE NOT RESPONSIBLE OR LIABLE FOR ANY DAMAGE OR LOSS CAUSED BY USE OF ANY CONTENT FROM THIS CODE. 
SPECIFICATION MENTIONED IN THIS PUBLICATION IS SUBJECT TO CHANGE WITHOUT NOTICE。
WITHOUT AUTHORIZATION, YOU ARE NOT ALLOWED TO COPY, DUPLICATE, OR MODIFY THIS CODE. 
ALL RIGHTS RESERVED.

************************************************/
#include "stm32f10x_lib.h"
#include "user_uart.h"
#include "spi.h"
#include "led.h"
#include "nrf24l01.h"
#include "interpreter.h"
#include "string.h"

u8 m_rxBuffer[256] = {0};
u8 m_txBuffer[256] = {0};
u8 m_rxCounter= 0;
u32 led_status=0;
bool m_rxFinish = FALSE;
extern u8 NRF_Dev_Addr[5];
extern u8 nrf_length;

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void delay(int delay);
void show_number(u8 num);
void delay_int(vu32 nCount); 

int main()
{
	RCC_Configuration();    //配置系统时钟
	NVIC_Configuration();   //配置  NVIC 和 Vector Table 

	//配置串口1
	UART1_Configuration();
	UART1_GPIO_Configuration();

	//配置SPI1
	SPI1_Init();
	SPI1_GPIO_Configuration();

	//配置LED
	LED_GPIO_Configuration();
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);	  //拉低点亮led

 	//配置无线
   	init_NRF24L01();

	while(1)
	{
		rf_checkEvent();
		inpertreter_checkFrame();

		led_status++;
		if (led_status & 0x8000) GPIO_SetBits(GPIOB, GPIO_Pin_12);
		else GPIO_ResetBits(GPIOB, GPIO_Pin_12);	  //拉低点亮led

	}
}

void show_number(u8 num)
{
	u8 num_high,num_low;

	num_high=(num & 0xf0)>>4;
	if (num_high<10) num_high=num_high+48;
	else num_high=num_high+55;
	Uart1_PutChar(num_high);

	num_low=num & 0x0f;
	if (num_low<10) num_low=num_low+48;
	else num_low=num_low+55;
	Uart1_PutChar(num_low);
}


void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  

  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void NVIC_Configuration(void)//配置  NVIC 和 Vector Table 
{
#ifdef  VECT_TAB_RAM

  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */

  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	UART1_NVIC_Configuration();//开串口1中断
}


void delay( int delay)
{
	int i;
	int j;
	for(j=0;j<delay;j++)
	{
		for (i=0; i<0xfffff; i++)
		{
			;
		}
	}
}

void delay_int(vu32 nCount) 
{
	for(; nCount != 0; nCount--);
}
