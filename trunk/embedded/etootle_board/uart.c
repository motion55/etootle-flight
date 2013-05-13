/*
  June 2012

  BaseFlightPlus Rev -

  An Open Source STM32 Based Multicopter

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick

  Designed to run on Naze32 Flight Control Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "uart.h"
#include "bsp.h"
#include "ring_buffer.h"
#include "string.h"
///////////////////////////////////////////////////////////////////////////////

/*
    DMA UART routines idea lifted from AutoQuad
    Copyright © 2011  Bill Nesbitt
*/

///////////////////////////////////////////////////////////////////////////////

#define UART_BUFFER_SIZE 64

uint8_t rxBuffer[UART_BUFFER_SIZE];
uint8_t txBuffer[UART_BUFFER_SIZE];

DECLARE_RING_BUFFER(usart_rx, 64, 16)

static uart_process_t  usart1_process;
static const uart_parser_t usart1_parser = {
    .header = 0xaa55,   // 55 aa header, LSB
    .header_len = 2,    // header length is 2 bytes
    .length_len = 1,    // data length field is 1 byte
    .cs_len = 0,        // no check sum
    .check_sum = 0,     // no check sum function
};
//static uart_process_t  usart2_process;

static void uart_data_ready(const void* pData, uint32_t len, void* context);
void uart1_init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    uart_process_init(&usart1_process, rxBuffer, UART_BUFFER_SIZE, &usart1_parser, USART1, uart_data_ready);
    ring_buf_init(usart_rx);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // UART1 TX -> PA.9,  RX -> PA.10

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // DMA TX Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // USART1 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    // Transmit DMA
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = sizeof(txBuffer);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA1_Channel4->CNDTR = 0;

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    USART_Cmd(USART1, ENABLE);
    
    usart1_process.txBusy = 0;
}

void USART1_IRQHandler(void)
{
    /* USART in mode Receiver --------------------------------------------------*/
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint16_t data = USART_ReceiveData(USART1);
        // use systick for time out
        uart_process_data(&usart1_process, (uint8_t)data);
        // when valid data received, we should set the tx pin to AF_1
        // this will disable SWD
        //USART_SendData(USART2, data);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET){
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
        usart1_process.txBusy = 0;
        // Tx data success
    }
    if(DMA_GetITStatus(DMA1_IT_TE4) == SET){
        DMA_ClearITPendingBit(DMA1_IT_TE4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
        usart1_process.txBusy = 0;
        // Tx data fail
    }
}

uint8_t uart1_send_data(const void* p, uint32_t len)
{
    //DMA_InitTypeDef         DMA_InitStructure;
    //DMA_Cmd(DMA1_Channel4, DISABLE);
    //USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
    
     //DMA_DeInit(DMA1_Channel4);
     //DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & USART1->DR;
     //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuffer;
     //DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
     //DMA_InitStructure.DMA_BufferSize = len;
     //DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
     //DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
     //DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
     //DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
     //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
     //DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
     
     //DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
     //DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    
    while(len){
        __IO uint32_t txTimeout = 65535;
        while(usart1_process.txBusy && txTimeout--);
        if(txTimeout == 0){
            return 1;
        }
        
        DMA1_Channel4->CNDTR = len;
        //DMA1_Channel4->CMAR = (uint32_t)txBuffer;
        //DMA1->IFCR = DMA1_FLAG_GL4;
        DMA_ClearFlag(DMA1_FLAG_GL4);
        
        if(len <= sizeof(txBuffer)){
            memcpy(txBuffer,p,len);
            len = 0;
        }else{
            DMA1_Channel4->CNDTR  = sizeof(txBuffer);
            memcpy(txBuffer, p, sizeof(txBuffer) );
            len -= sizeof(txBuffer);
            p = ((const char*)p) + sizeof(txBuffer);
        }
        //USART1->CR3 |= USART_DMAReq_Tx;
        //USART1->SR = (uint16_t)~USART_FLAG_TC;
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
        USART_ClearFlag(USART1, USART_FLAG_TC);
        
        usart1_process.txBusy = 1;
        //DMA_ITConfig(DMA1_Channel4, DMA_IT_TC | DMA_IT_TE, ENABLE);
        //DMA_Cmd(DMA1_Channel4, ENABLE);
        DMA1_Channel4->CCR |= DMA_CCR1_EN;
    }
    return 0;
}

static void uart_data_ready(const void* pData, uint32_t len, void* context)
{
    uint8_t buf[64] = {len};
    memcpy(buf+1, pData, len);
    ring_buf_push(usart_rx, buf, len + 1);
}

void rf_handleReceivedData(const uint8_t * packet,int32_t length);
void uart_checkEvent(void)
{
    uint8_t buf[64];
    if(ring_buf_pop(usart_rx, buf, 64)){
        rf_handleReceivedData(buf+1, buf[0]);
    }
}

uint32_t uart_transmit(const uint8_t * data,uint32_t len)
{
    uint8_t buf[64];
    len = uart_pack_data(&usart1_process, data, len, buf, 64);
    return uart1_send_data(buf, len);
}

