#include "stm32f10x_conf.h"
#include "stm32f10x_lib.h"

void NRF_Interrupt_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;//EXTI0_IRQChannel
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

 	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

  	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
  	EXTI_InitStructure.EXTI_Line = EXTI_Line5; //	 EXTI_Line0
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}





