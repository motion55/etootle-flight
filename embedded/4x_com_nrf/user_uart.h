/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _USER_UART_H
#define _USER_UART_H

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_lib.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define uint32_t u32
#define uint8_t u8

void UART1_NVIC_Configuration(void);
void UART1_Configuration(void);
void UART1_GPIO_Configuration(void);
u8 Uart1_PutChar(u8 ch);
void Uart1_PutString(u8* buf , u8 len);

//////////////////////////////////////////////////
void UART2_NVIC_Configuration(void);
void UART2_Configuration(void);
void UART2_GPIO_Configuration(void);
u8 Uart2_PutChar(u8 ch);
void Uart2_PutString(u8* buf , u8 len);


uint32_t uart_receiveBufferSizeIdle(void);
uint32_t uart_receiveBufferSizeOccupy(void);
uint8_t uart_receivePushToBuffer(uint8_t data);

uint32_t uart_transmitBufferSizeIdle(void);
uint32_t uart_transmitBufferSizeOccupy(void);
void uart_transmitAByte(void);

uint8_t serial_transmit(const void * data,uint8_t len);

#endif /* _USER_UART_H */
