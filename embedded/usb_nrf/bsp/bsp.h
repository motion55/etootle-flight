#ifndef BSP_H
#define BSP_H
#include "gpio.h"
#include "spi.h"

#define NRF_TRANSFER_BYTE(x)    spi2_transfer_byte(x)

#define NRF_INIT()     \
    do{\
        setup_io_nrf24l01();\
        spi2_init();\
    }while(0)

void delay_ms(uint32_t ms);
void delay_us(uint32_t usec);
uint32_t get_tick_count(void);
uint32_t current_us(void);
void setup_systick(void);
void enable_tick_count(void);
#endif
