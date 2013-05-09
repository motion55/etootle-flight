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

// bsp/nrf24l01p.c
// 2012-03-30 08:56:35

#include <stdint.h>
#include "rf.h"
#include "rf_hal.h"

#define _BV(n) (1<<(n))

/*
 * address。*/
#define RF_LOCAL_ADDRESS ((uint32_t)0x5AB76476)

/*
 * command。*/
#define RF_CMD_R_REGISTER   0x00/*0b00000000*/  /* 读寄存器，0b000aaaaa，a为地址。 */
#define RF_CMD_W_REGISTER   0x20/*0b00100000*/  /* 写寄存器，0b001aaaaa，a为地址。 */
    #define RF_CMD_RWR_MASK 0x1F/*0b00011111*/  /* 寄存器地址掩码，0bcccaaaaa */
#define RF_CMD_R_RX_PAYLOAD 0x61/*0b01100001*/  /* 获取数据。 */
#define RF_CMD_W_TX_PAYLOAD 0xA0/*0b10100000*/  /* 推送数据。 */
#define RF_CMD_W_ACK_PAYLOAD 0xA8/*0b10100000*/  /* 推送ACK数据。 */
#define RF_CMD_FLUSH_TX     0xE1/*0b11100001*/  /* 清空发送缓存。 */
#define RF_CMD_FLUSH_RX     0xE2/*0b11100010*/  /* 清空接收缓存。 */
#define RF_CMD_R_RX_PL_WID  0x60/*0b01100000*/  /* 获取接收到的数据的长度。 */
#define RF_CMD_NOP          0xFF/*0b11111111*/  /* 空操作，用来读STATUS寄存器。 */

/*
 * regester map*/
#define RF_REG_CONFIG 0x00
    #define RF_REG_CONFIG_MASK_RX_DR    _BV(6) /* 接收中断。 */
    #define RF_REG_CONFIG_MASK_TX_DS    _BV(5) /* 发送中断。 */
    #define RF_REG_CONFIG_MASK_MAX_RT   _BV(4) /* 重发失败中断。 */
    #define RF_REG_CONFIG_EN_CRC        _BV(3) /* 使用CRC。 */
    #define RF_REG_CONFIG_CRCO          _BV(2) /* 使用2位CRC。 */
    #define RF_REG_CONFIG_PWR_UP        _BV(1) /* 马上进入Standby1状态。 */
    #define RF_REG_CONFIG_PRIM_RX       _BV(0) /* 是否作为接收器。 */
    #define RF_REG_CONFIG_DEFAULT_R        /* 默认。 */ \
        (RF_REG_CONFIG_CRCO | RF_REG_CONFIG_EN_CRC | \
        RF_REG_CONFIG_PWR_UP | RF_REG_CONFIG_PRIM_RX)
    #define RF_REG_CONFIG_DEFAULT_T        /* 默认。 */ \
        (RF_REG_CONFIG_CRCO | RF_REG_CONFIG_EN_CRC | \
        RF_REG_CONFIG_PWR_UP)
#define RF_REG_EN_AA 0x01
    #define RF_REG_EN_AA_ENAA_P0    _BV(0) /* 0通道自动ACK。 */
    #define RF_REG_EN_AA_DEFAULT    RF_REG_EN_AA_ENAA_P0 /* 默认。 */
#define RF_REG_EN_RXADDR 0x02
    #define RF_REG_EN_RXADDR_ERX_P0 _BV(0) /* 使用0通道。 */
    #define RF_REG_EN_RXADDR_DEFAULT RF_REG_EN_RXADDR_ERX_P0 /* 默认。 */
#define RF_REG_SETUP_AW 0x03
    #define RF_REG_SETUP_AW_N(n)    (n-2)  /* 使用n位地址，n:[3,5] */
    #define RF_REG_SETUP_AW_DEFAULT RF_REG_SETUP_AW_N(4) /* 默认 */
#define RF_REG_SETUP_RETR 0x04
    #define RF_REG_SETUP_RETR_ARD(n)    ((uint8_t)n<<4)   /* (n+1)*250μs没收到ACK就重发。 */
    #define RF_REG_SETUP_RETR_ARC(n)    ((uint8_t)n&0x0F) /* 最多重发n次。 */
    #define RF_REG_SETUP_RETR_DEFAULT   /* 默认 */ \
        (RF_REG_SETUP_RETR_ARD(3) | RF_REG_SETUP_RETR_ARC(4))
#define RF_REG_RF_CH 0x05
    #define RF_REG_RF_CH_MHZ(n)      ((uint8_t)(n-2400)&(~_BV(7))) /* 设置频率，Mhz，[2400,2525]。*/
    #define RF_REG_RF_CH_DEFAULT     RF_REG_RF_CH_MHZ(2400) /* 默认 */
#define RF_REG_RF_SETUP 0x06
    #define RF_REG_RF_SETUP_CONT_WAVE   _BV(7) /* 允许连续发送。 */
    #define RF_REG_RF_SETUP_RATE_250K   _BV(5) /* 设置速率，250kBps。 */
    #define RF_REG_RF_SETUP_RATE_1M     0      /* 设置速率，1MBps。 */
    #define RF_REG_RF_SETUP_RATE_2M     _BV(3) /* 设置速率，2MBps。 */
    #define RF_REG_RF_SETUP_PWR(n)      (n<<1) /* 设置发射功率，(-6*(3-n))dBm。 */
    #define RF_REG_RF_SETUP_DEFAULT     /* 默认 */ \
        (RF_REG_RF_SETUP_RATE_2M | RF_REG_RF_SETUP_PWR(3))
#define RF_REG_STATUS 0x07
    #define RF_REG_STATUS_RX_DR     _BV(6) /* 接收到数据。 */
    #define RF_REG_STATUS_TX_DS     _BV(5) /* 发送完数据。 */
    #define RF_REG_STATUS_MAX_RT    _BV(4) /* 重发超过最大次数。 */
    #define RF_REG_STATUS_TX_FULL   _BV(0) /* 发送缓冲区满。 */
#define RF_REG_FIFO_STATUS 0x17
    #define RF_REG_FIFO_STATUS_TX_REUSE     _BV(6) /*  */
    #define RF_REG_FIFO_STATUS_TX_FULL      _BV(5) /* 1:发送队列满。 */
    #define RF_REG_FIFO_STATUS_TX_EMPTY     _BV(4) /* 1:发送队列空。 */
    #define RF_REG_FIFO_STATUS_RX_FULL      _BV(1) /* 1:接收队列满。 */
    #define RF_REG_FIFO_STATUS_RX_EMPTY     _BV(0) /* 1:接收队列空。 */
#define RF_REG_DYNPD 0x1C
    #define RF_REG_DYNPD_DPL_P0 _BV(0) /* 设置通道0为动态数据长度。 */
    #define RF_REG_DYNPD_DEFAULT RF_REG_DYNPD_DPL_P0 /* 默认。 */
#define RF_REG_FEATURE 0x1D
    #define RF_REG_FEATURE_EN_DPL   _BV(2) /* 允许动态数据长度。 */
    #define RF_REG_FEATURE_EN_ACKPL _BV(1) /* 允许ACK数据。 */
    #define RF_REG_FEATURE_DEFAULT (RF_REG_FEATURE_EN_DPL | RF_REG_FEATURE_EN_ACKPL)/* 默认。 */
#define RF_REG_RX_ADDR_P0 0x0A /* 接收的地址 */
#define RF_REG_TX_ADDR    0x10 /* 发送目的地址 */
#define RF_REG_RX_PW_P0   0x11 /* 0接收通道收到的数据的长度。 */

int32_t     rf_init(void);
int32_t     rf_transmit(const uint8_t * data,uint32_t len);
void        rf_startReceive(void);
void        rf_stopReceive(void);
void        rf_checkEvent(void);
static void     rf_writeRegester(uint8_t addr,uint8_t val);
static uint8_t  rf_readRegester(uint8_t addr);
//static void     rf_writeAddress(uint8_t addr_reg,uint32_t addr_rf);
static void     rf_writeData(const uint8_t * data,uint8_t len);
static uint8_t  rf_readData(uint8_t * data,uint8_t * len);
static void     rf_getStatus(uint8_t * status,uint8_t * fifo_status);
static uint8_t  rf_getReceivedDataLength(void);
static void     rf_flushTxFifo(void);
static void     rf_flushRxFifo(void);

// 返回0表示成功。
int32_t rf_init(void)
{
    // 初始化针脚。
    rf_hal_init();
    //
    // 初始化寄存器。
    const static uint8_t reg_value[][2] =
    {
        {RF_REG_CONFIG      ,RF_REG_CONFIG_DEFAULT_R  },
        {RF_REG_EN_RXADDR   ,RF_REG_EN_RXADDR_DEFAULT },
        {RF_REG_EN_AA       ,RF_REG_EN_AA_DEFAULT     },
        {RF_REG_SETUP_AW    ,RF_REG_SETUP_AW_DEFAULT  },
        {RF_REG_SETUP_RETR  ,RF_REG_SETUP_RETR_DEFAULT},
        {RF_REG_RF_CH       ,RF_REG_RF_CH_DEFAULT     },
        {RF_REG_RF_SETUP    ,RF_REG_RF_SETUP_DEFAULT  },
        {RF_REG_DYNPD       ,RF_REG_DYNPD_DEFAULT     },
        {RF_REG_FEATURE     ,RF_REG_FEATURE_DEFAULT   },
    };
    for(uint32_t i=0;i<sizeof(reg_value)/2;i++)
        rf_writeRegester(reg_value[i][0],reg_value[i][1]);
    for(uint32_t i=0;i<sizeof(reg_value)/2;i++)
        if(rf_readRegester(reg_value[i][0]) != reg_value[i][1])
            return 1 + reg_value[i][0] * 1e3 + reg_value[i][1]*1e6;
    //
    rf_flushTxFifo();
    rf_flushRxFifo();
    //
    return 0;
}

/*
 * 发送数据。
 * 返回值：
 *  0：已经送到RF芯片。
 *  1：RF芯片缓冲区已满。*/
int32_t rf_transmit(const uint8_t * data,uint32_t len)
{
    /*
     * 获取状态。*/
    rf_hal_spiBegin();
    uint8_t status = rf_hal_spiSwap(RF_CMD_NOP);
    rf_hal_spiEnd();
    /*
     * 判断是否有空位。*/
    if(status & RF_REG_STATUS_TX_FULL)
        return 1;
    //
    rf_hal_ceLow();
    //rf_writeRegester(RF_REG_CONFIG,RF_REG_CONFIG_DEFAULT_T);
    rf_writeData(data,len);
    rf_hal_ceHigh();
    //
    return 0;
}

void rf_startReceive(void)
{
    rf_hal_ceLow();
    //rf_writeRegester(RF_REG_CONFIG,RF_REG_CONFIG_DEFAULT_R);
    rf_hal_ceHigh();
}

void rf_stopReceive(void)
{
    rf_hal_ceLow();
}

void rf_writeRegester(uint8_t addr,uint8_t val)
{
    rf_hal_spiBegin();
    //
    rf_hal_spiSwap(RF_CMD_W_REGISTER | (addr & RF_CMD_RWR_MASK));
    rf_hal_spiSwap(val);
    //
    rf_hal_spiEnd();
}

uint8_t rf_readRegester(uint8_t addr)
{
    rf_hal_spiBegin();
    //
    rf_hal_spiSwap(RF_CMD_R_REGISTER | (addr & RF_CMD_RWR_MASK));
    uint8_t rt = rf_hal_spiSwap(0);
    //
    rf_hal_spiEnd();
    //
    return rt;
}

/*
void rf_writeAddress(uint8_t addr_reg,uint32_t addr_rf)
{
    rf_hal_spiBegin();
    //
    rf_hal_spiSwap(RF_CMD_W_REGISTER | (addr_reg & RF_CMD_RWR_MASK));
    rf_hal_spiSwap((uint8_t)addr_rf);
    addr_rf >>= 8;
    rf_hal_spiSwap((uint8_t)addr_rf);
    addr_rf >>= 8;
    rf_hal_spiSwap((uint8_t)addr_rf);
    addr_rf >>= 8;
    rf_hal_spiSwap((uint8_t)addr_rf);
    //
    rf_hal_spiEnd();
}
*/

void rf_checkEvent(void)
{
    /*
     * 中断线被拉低，表示有中断。*/
    if(! rf_hal_irqIsHigh())
    {
        /*
         * 获取状态。*/
        uint8_t status = 0;
        uint8_t fifo_status = 0;
        rf_getStatus(&status,&fifo_status);
        /*
         * 收到数据。 */
        if(status & RF_REG_STATUS_RX_DR)
        {
            rf_stopReceive();
            //
			uint8_t max_times = 3;
            while(!(fifo_status & RF_REG_FIFO_STATUS_RX_EMPTY) && max_times--)
            {
                /*
                 * 取出并处理数据。*/
                uint8_t buf[32];
                uint8_t len = 0;
                if(rf_readData(buf,&len) != 0)
                    break;
                rf_handleReceivedData(buf,len);
                /*
                 * 获取状态。*/
                rf_getStatus(&status,&fifo_status);
            }
            //
            rf_startReceive();
        }
        /*
         * 发送完数据。 */
        if(status & RF_REG_STATUS_TX_DS)
        {
            /*
             * 没有等待的数据就停止发送。 */
            if(fifo_status & RF_REG_FIFO_STATUS_TX_EMPTY)
                rf_startReceive();
        }
        /*
         * 重发次数超额。 */
        if(status & RF_REG_STATUS_MAX_RT)
        {
            /*
             * 清空缓冲区。*/
            rf_flushTxFifo();
            rf_startReceive();
        }
        /*
         * 取消中断标识。*/
        rf_writeRegester(RF_REG_STATUS,status);
    }
}

void rf_writeData(const uint8_t * data,uint8_t len)
{
    rf_hal_spiBegin();
    //
    //rf_hal_spiSwap(RF_CMD_W_TX_PAYLOAD);
    rf_hal_spiSwap(RF_CMD_W_ACK_PAYLOAD);
    for(uint8_t i=0;i<len;i++)
        rf_hal_spiSwap(data[i]);
    //
    rf_hal_spiEnd();
}

/*
 * 取出数据。
 * 返回值：
 *  0：成功。
 *  1：失败，并且已经清空接收缓冲区。*/
uint8_t rf_readData(uint8_t * p_data,uint8_t * p_len)
{
    /*
     * 检查数据长度有效性。*/
    uint8_t len = rf_getReceivedDataLength();
    if(len > 32)
    {
        /*
         * 数据无效，清空缓冲区。*/
         rf_flushRxFifo();
        return 1;
    }
    /*
     * 取出数据。*/
    rf_hal_spiBegin();
    rf_hal_spiSwap(RF_CMD_R_RX_PAYLOAD);
    for(uint8_t i=0;i<len;i++)
        p_data[i] = rf_hal_spiSwap(0);
    rf_hal_spiEnd();
    //
    *p_len = len;
    return 0;
}

uint8_t rf_getReceivedDataLength(void)
{
    uint8_t rt = 0;
    //
    rf_hal_spiBegin();
    rf_hal_spiSwap(RF_CMD_R_RX_PL_WID);
    rt = rf_hal_spiSwap(0);
    rf_hal_spiEnd();
    //
    return rt;
}

void rf_getStatus(uint8_t * status,uint8_t * fifo_status)
{
    rf_hal_spiBegin();
    *status = rf_hal_spiSwap(RF_CMD_R_REGISTER | (RF_REG_FIFO_STATUS & RF_CMD_RWR_MASK));
    *fifo_status = rf_hal_spiSwap(0);
    rf_hal_spiEnd();
}

void rf_flushRxFifo(void)
{
    rf_hal_spiBegin();
    rf_hal_spiSwap(RF_CMD_FLUSH_RX);
    rf_hal_spiEnd();
}

void rf_flushTxFifo(void)
{
    rf_hal_spiBegin();
    rf_hal_spiSwap(RF_CMD_FLUSH_TX);
    rf_hal_spiEnd();
}
