#ifndef BSP_H
#define BSP_H
#include "string.h"
#include "gpio.h"
#include "spi.h"


#define NRF_TRANSFER_BYTE(x)    spi1_transfer_byte(x)

#define NRF_INIT()     \
    do{\
        setup_io_nrf24l01();\
        spi1_init();\
    }while(0)


//void memory_copy(void* dest, const void* src, uint32_t len);
#define memory_copy(dest,src,len) memcpy((dest),(src),(len))
#define memory_set(dest,val,len) memset((dest),(val),(len))
void delay_ms(uint32_t ms);
void delay_us(uint32_t usec);
uint32_t get_tick_count(void);
uint32_t current_us(void);
void setup_systick(void);
void enable_tick_count(void);
#endif
