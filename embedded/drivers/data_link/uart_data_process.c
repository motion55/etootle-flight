#include "uart_data_process.h"
#include "string.h"



#define    IDLE     0
#define    LEN      1
#define    DATA     2
#define    CS       3


void uart_process_init(
    uart_process_t* process,
    uint8_t* buffer,
    uint32_t buffer_size,
    const uart_parser_t* parser,
    void* context,
    pfn_uart_data_ready data_ready)
{
    process->state = IDLE;
    process->buffer = buffer;
    process->buffer_size = buffer_size;
    process->len = 0;
    process->bufIndex = 0;
    process->head = 0;
    process->uart_data_ready = data_ready;
    process->cs = 0;
    process->context = context;
    process->parser = parser;
}

void uart_process_data(uart_process_t* process, uint8_t data)
{
    switch(process->state){
        case IDLE:
        {
            uint32_t len = process->parser->header_len;
            uint32_t mask = (1<<(len*8))-1;
            process->head >>= 8;
            process->head |= (data<< ((len-1)*8));
            if( ( (process->head ^ process->parser->header) & mask) == 0){
                process->state = LEN;
                process->len = 0;
                process->bufIndex = 0;
            }
        }
        break;
        case LEN:
        {
            uint32_t len = process->parser->length_len;
            process->len >>= 8;
            process->len |= data<<((len-1)*8);
            process->bufIndex++;
            if(process->bufIndex >= len){
                if( process->len + process->parser->cs_len < process->buffer_size ){
                    process->state = DATA;
                    process->bufIndex = 0;
                }
            }
        }
        break;
        case DATA:
        {
            process->buffer[process->bufIndex++] = data;
            if(process->bufIndex >= process->len){
                if(process->parser->cs_len){
                    process->cs = 0;
                    process->bufIndex = 0;
                    process->state = CS;
                }else{
                    process->state = IDLE;
                    if(process->uart_data_ready){
                        process->uart_data_ready(process->buffer,process->len,process->context);
                    }
                }
            }
        }
        break;
        case CS:
        {
            uint32_t len = process->parser->cs_len;
            process->cs >>= 8;
            process->cs |= data<<((len-1)*8);
            process->bufIndex++;
            if(process->bufIndex >= len){
                uint32_t cs = process->cs;
                if(process->parser->check_sum){
                    cs = process->parser->check_sum(process->buffer,process->len,process->parser);
                }
                if(cs == process->cs){
                    if(process->uart_data_ready){
                        process->uart_data_ready(process->buffer,process->len,process->context);
                    }
                }
                process->state = IDLE;
            }
        }
        break;
    }
}

uint32_t uart_pack_data(uart_process_t* process, const uint8_t* data_in, uint32_t in_len, uint8_t* data_out, uint32_t out_len)
{
    uint32_t len = in_len + process->parser->header_len + process->parser->length_len + process->parser->cs_len;
    uint32_t cs = 0;
    if(out_len < len) return 0;
    if(process->parser->check_sum && process->parser->cs_len){
        cs = process->parser->check_sum(data_in, in_len, process->parser);
    }
    len = process->parser->header_len + process->parser->length_len;
    // overlap memcpy
    memmove(data_out + len, data_in, in_len);
    
    memcpy(data_out, &process->parser->header, process->parser->header_len);
    len = process->parser->header_len;
    memcpy(data_out + len, &in_len, process->parser->length_len);
    len += process->parser->length_len;
    //memcpy(data_out + len, data_in, in_len);
    len += in_len;
    memcpy(data_out + len, &cs, process->parser->cs_len);
    len += process->parser->cs_len;
    return len;
}


