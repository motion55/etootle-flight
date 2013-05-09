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

// app/communication/transmit.c
// 2013-1-8 13:10:44
// js200300953

#include "bsp/rf.h"
#include "protocol.h"
#include "transmit.h"
#include "app/math/interface.h"

#define TRANSMIT_BUFFER_AMOUNT 16

static uint8_t  transmit_packet[TRANSMIT_BUFFER_AMOUNT][TRANSMIT_PACKET_LENGTH_MAX];
static uint32_t transmit_length[TRANSMIT_BUFFER_AMOUNT];
static int32_t  transmit_head; // 指向下一空单元。
static int32_t  transmit_tail; // 指向最后一个有效单元。
static int32_t  transmit_isFull;
static int32_t  transmit_isPacketing;
static uint16_t transmit_currentCrc;

void    transmit_init(void);
void    transmit_checkEvent(void);
int32_t transmit_packetBegin(uint8_t packetType);
int32_t transmit_packetContent(const void * part,int32_t length);
int32_t transmit_packetEnd(void);

void transmit_init(void)
{
    transmit_head           = 0;
    transmit_tail           = 0;
    transmit_isFull         = 0;
    transmit_isPacketing    = 0;
    transmit_currentCrc     = 0;
    //
    for(int32_t i=0;i<TRANSMIT_BUFFER_AMOUNT;i++)
    {
        for(int32_t j=0;j<TRANSMIT_PACKET_LENGTH_MAX;j++)
            transmit_packet[i][j] = 0;
        transmit_length[i] = 0;
    }
}

void transmit_checkEvent(void)
{
    if(transmit_head == transmit_tail
            && ! transmit_isFull)
        return; // 缓冲区空。
    //
    // 发送数据，并推进尾索引。
    int32_t rs = rf_transmit(transmit_packet[transmit_tail],
            transmit_length[transmit_tail]);
    if(rs)
        return;
    //
    // 已放到rf发送缓冲区，推进队列。
    transmit_tail = (transmit_tail + 1) % TRANSMIT_BUFFER_AMOUNT;
    transmit_isFull = 0;
}

// 开始一个新的包。
int32_t transmit_packetBegin(uint8_t packetType)
{
    if(transmit_isFull)
        return 1;
    //
    transmit_packet[transmit_head][0] = PROTOCOL_VERSION;
    transmit_packet[transmit_head][1] = packetType;
    transmit_length[transmit_head] = 2;
    //
    uint8_t tmp[] = {PROTOCOL_VERSION,packetType};
    transmit_currentCrc = math_crc16(0,tmp,sizeof(tmp));
    //
    transmit_isPacketing = 1;
    return 0;
}

// 填充包内容。可多次调用。
int32_t transmit_packetContent(const void * part,int32_t length)
{
    if(! transmit_isPacketing)
        return 1;
    //
    int32_t length_before = transmit_length[transmit_head];
    //
    // 判断是否合理。-2以保留CRC的位置。
    if((! transmit_isPacketing) || length_before+length > TRANSMIT_PACKET_LENGTH_MAX-2)
        return 1;
    //
    // 复制数据。
    for(int32_t i=0;i<length;i++)
        transmit_packet[transmit_head][length_before+i] = ((uint8_t *)part)[i];
    transmit_length[transmit_head] += length;
    //
    // 更新CRC。
    transmit_currentCrc = math_crc16(transmit_currentCrc,part,length);
    //
    return 0;
}

// 结束当前包，并放入到发送队列里。
int32_t transmit_packetEnd(void)
{
    if(! transmit_isPacketing)
        return 1;
    //
    // 把CRC放到包后面。
    int32_t length_before = transmit_length[transmit_head];
    transmit_packet[transmit_head][length_before + 0]
        = ((uint8_t *)&transmit_currentCrc)[1];
    transmit_packet[transmit_head][length_before + 1] 
        = ((uint8_t *)&transmit_currentCrc)[0];
    transmit_length[transmit_head] += 2;
    //
    // 推进队列，相当于把当前包加入到队列。
    transmit_head = (transmit_head + 1) % TRANSMIT_BUFFER_AMOUNT;
    if(transmit_head == transmit_tail)
        transmit_isFull = 1;
    //
    transmit_isPacketing = 0;
    return 0;
}
