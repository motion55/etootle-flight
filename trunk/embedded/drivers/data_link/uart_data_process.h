#ifndef UART_DATA_PROCESS_H
#define UART_DATA_PROCESS_H

#include "stdint.h"

#ifndef DATA_MAX_LEN
#define DATA_MAX_LEN  64
#endif
#ifndef NEED_TIMER
#define NEED_TIMER 0
#endif
struct uart_parser_t;
typedef uint32_t (*pfn_check_sum)(const uint8_t* data, uint32_t len, const struct uart_parser_t* parser);

typedef struct uart_parser_t
{
    uint32_t  header;
    uint32_t  header_len;
    uint32_t  length_len;
    uint32_t  cs_len;
    pfn_check_sum check_sum;
}uart_parser_t;

typedef void (*pfn_uart_data_ready)(const void* pData, uint32_t len, void* context);

typedef struct uart_process_t
{
    uint8_t* buffer;
    uint32_t buffer_size;
    uint32_t head;
    uint32_t len;
    uint32_t bufIndex;
    uint32_t cs;
    void* context;
    const uart_parser_t* parser;
    pfn_uart_data_ready uart_data_ready;
    uint8_t state;
    uint8_t txBusy;
}uart_process_t;

void uart_process_init(uart_process_t* process, uint8_t* buffer, uint32_t buffer_size, const uart_parser_t* parser, void* context,pfn_uart_data_ready data_ready);
void uart_process_data(uart_process_t* process, uint8_t data);
uint32_t uart_pack_data(uart_process_t* process, const uint8_t* data_in, uint32_t in_len, uint8_t* data_out, uint32_t out_len);

// return value: 0:     not timeout
//               non 0: timeout
int dp_is_timeout(void) __attribute__((weak));
void dp_start_timer(void) __attribute__((weak));
void dp_stop_timer(void) __attribute__((weak));

#endif
