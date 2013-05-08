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

// app/refresher/refresher.c
// 2013-2-21 21:04:21
// js200300953

#include <stm32f10x.h>
#include <app/math/interface.h>
#include "refresher.h"
#include "flash_map.h"

// app段的块映射。
// app的每一个块都对应refresher_appWrittenBitMap的一个位。
int32_t refresher_appBlockBitMap[MAP_APP_SIZE_BLOCK/(sizeof(int32_t)*8) + 1];
int32_t refresher_appSize_block = 0; // app的长度，以块为单位。
int32_t refresher_appCrc = 0; // app的校验码。
int32_t refresher_checkCompleteStartIndex = 0; // 检查是否完成时，作为起始索引。
int32_t refresher_writtenAmount_block = 0; // 累加已写入的块数。

#define STM32_SRAM_BITMAP_BASE 0x22000000
#define REFRESHER_APP_BLOCK_BITMAP_BASE_INT32 ((int32_t *)((int32_t)refresher_appBlockBitMap | STM32_SRAM_BITMAP_BASE))
// #define refresher_setBit(index) REFRESHER_APP_BLOCK_BITMAP_BASE_INT32[index]=1
// #define refresher_getBit(index) REFRESHER_APP_BLOCK_BITMAP_BASE_INT32[index]
#define refresher_setBit(index) refresher_appBlockBitMap[(index)/32] |= (1<<(index)%32)
#define refresher_getBit(index) (refresher_appBlockBitMap[(index)/32] & (1<<(index)%32))

int32_t refresher_init(void);
int32_t refresher_checkApp(void);
int32_t refresher_setAppSizeCrc(uint32_t size_block,uint32_t crc);
int32_t refresher_eraseApp(void);
int32_t refresher_write(const uint32_t *data,int32_t length,int32_t blockIndexOfFirst);
static int32_t refresher_skipWrittenBlock(void);
int32_t refresher_isWriteComplete(void);
int32_t refresher_getRemainBlock(uint8_t * buffer,int32_t bufferLength);
int32_t refresher_getWrittenAmount_block(void);
int32_t refresher_jumpToApp(void);

int32_t refresher_init(void)
{
    return 0;
}

// 检查app是否有效。
// 返回值：
//    0 => app有效。
//   -1 => app长度不合理。
//   -2 => 校验失败。
int32_t refresher_checkApp(void)
{
    // 检查app有效长度的有效性。
    uint32_t appValidBlockAmount = *MPA_APP_VALID_BLOCK_AMOUNT_ADDR_UINT32;
    if(appValidBlockAmount <= 0 || appValidBlockAmount > MAP_APP_SIZE_BLOCK)
        return -1;
    //
    // 计算app的crc，并与MPA_APPCRC_ADDR_INT32里的值比较。
    uint32_t crc_cal = math_crc16(0,MAP_APP_BASE_BYTE,appValidBlockAmount*MAP_BLOCK_SIZE_BYTE);
    if(crc_cal != (uint32_t)*MPA_APP_CRC_ADDR_UINT32)
        return -2;
    //
    return 0;
}

int32_t refresher_setAppSizeCrc(uint32_t size_block,uint32_t crc)
{
    if(size_block > MAP_APP_SIZE_BYTE)
        return -1;
    //
    refresher_appSize_block = size_block;
    refresher_appCrc = crc;
    //
    return 0;
}

int32_t refresher_eraseApp(void)
{
    // 判断refresher_appSizeByte是否合理。
    if(refresher_appSize_block < 0 || refresher_appSize_block > MAP_APP_SIZE_BLOCK)
        return -1;
    //
    // 重置位映射。即标记全部块都未写入。
    for(int32_t i=0;i<sizeof(refresher_appBlockBitMap)/sizeof(refresher_appBlockBitMap[0]);i++)
        refresher_appBlockBitMap[i] = 0;
    refresher_writtenAmount_block = 0;
    refresher_checkCompleteStartIndex = 0;
    //
    // 解锁flash。
    FLASH_Unlock();
    //
    // 根据refresher_appSizeByte擦除需要的块，如果全部擦除浪费时间。
    int32_t pageAmount = (refresher_appSize_block / MPA_FLASH_PAGE_SIZE_BLOCK) + 1;
    for(int32_t i=0;i<pageAmount;i++)
        FLASH_ErasePage((uint32_t)(MAP_APP_BASE_PAGE+i));
    //
    // 擦除app最后一个page，用于存放len和crc。
    if(pageAmount < MAP_APP_SIZE_PAGE)
        FLASH_ErasePage((uint32_t)(MAP_APP_BASE_PAGE+MAP_APP_SIZE_PAGE-1));
    //
    // 顺便写入len和crc。
    FLASH_ProgramWord((uint32_t)MPA_APP_VALID_BLOCK_AMOUNT_ADDR_UINT32   ,refresher_appSize_block);
    FLASH_ProgramWord((uint32_t)MPA_APP_CRC_ADDR_UINT32                  ,refresher_appCrc);
    //
    FLASH_Lock();
    //
    return 0;
}

int32_t refresher_write(const uint32_t *data,int32_t length,int32_t blockIndexOfFirst)
{
    FLASH_Unlock();
    //
    for(int32_t i=0;i<length;i++)
    {
        // 已经写过就不再写了。
        if(refresher_getBit(blockIndexOfFirst+i))
            continue;
        //
        // 写入flash。
        FLASH_ProgramWord((uint32_t)(MAP_APP_BASE_BLOCK + (blockIndexOfFirst+i)),data[i]);
        //
        // 标记位映射为已写。
        refresher_setBit(blockIndexOfFirst+i);
        refresher_writtenAmount_block ++;
    }
    //
    FLASH_Lock();
    //
    return 0;
}

// 检查app是否写入完成。
// 返回refresher_checkCompleteStartIndex跨过的块数。
// 如果未完成，会把refresher_checkCompleteStartIndex指向下一个未完成的块。
// 这样提高多次查询的效率。
static int32_t refresher_skipWrittenBlock(void)
{
    int32_t old_index = refresher_checkCompleteStartIndex;
    //
    for(;
        refresher_checkCompleteStartIndex < refresher_appSize_block;
        refresher_checkCompleteStartIndex ++)
    {
        if(! refresher_getBit(refresher_checkCompleteStartIndex))
            return refresher_checkCompleteStartIndex-old_index;
    }
    for(refresher_checkCompleteStartIndex = 0;
        refresher_checkCompleteStartIndex<old_index;
        refresher_checkCompleteStartIndex++)
    {
        if(! refresher_getBit(refresher_checkCompleteStartIndex))
            return ((refresher_appSize_block-old_index) + refresher_checkCompleteStartIndex);
    }
    //
    return refresher_appSize_block;
}

int32_t refresher_isWriteComplete(void)
{
    return refresher_skipWrittenBlock() >= refresher_appSize_block;
}

// 按[{index_uint16,length_uint16},...]打包未写区域。返回填入buffer的字节数。
int32_t refresher_getRemainBlock(uint8_t * buffer,int32_t bufferLength)
{
    if(bufferLength < 4)
        return -1;
    //
    if(refresher_isWriteComplete())
        return 0;
    //
    int32_t curLen = 0; // 放入buffer的连续未写区域的数目，返回值。
    int32_t step   = 0; // 累加步数，用于判断结束。
    //
    while(1)
    {
        // 跳过已写过的块。
        step += refresher_skipWrittenBlock();
        if(step >= refresher_appSize_block)
            break;
        //
        // 来到这里，表示有未写的块，而且refresher_checkCompleteStartIndex指向下一个未写的块。
        // 保存refresher_checkCompleteStartIndex，并寻找下一个已写的块。
        uint16_t index = (uint16_t)refresher_checkCompleteStartIndex;
        for(;
            refresher_checkCompleteStartIndex < refresher_appSize_block;
            refresher_checkCompleteStartIndex ++)
        {
            if(refresher_getBit(refresher_checkCompleteStartIndex))
                break;
        }
        step += refresher_checkCompleteStartIndex - index;
        // refresher_checkCompleteStartIndex已经指向下一个已写的块，或者到达末尾。
        //
        // 计算刚刚连续的未写的块的长度。
        uint16_t len = refresher_checkCompleteStartIndex - index;
        //
        // 如果refresher_checkCompleteStartIndex到达末尾，要回到前面。
        if(refresher_checkCompleteStartIndex >= refresher_appSize_block)
            refresher_checkCompleteStartIndex = 0;
        //
        // 保存index和len到buffer。
        buffer[curLen ++] = ((uint8_t *)&index)[0];
        buffer[curLen ++] = ((uint8_t *)&index)[1];
        buffer[curLen ++] = ((uint8_t *)&len)[0];
        buffer[curLen ++] = ((uint8_t *)&len)[1];
        //
        // 判断是否有足够空间装下一个。
        if(bufferLength < (curLen+2+2))
            break;
    }
    //
    return curLen;
}

int32_t refresher_getWrittenAmount_block(void)
{
    return refresher_writtenAmount_block;
}

int32_t refresher_jumpToApp(void)
{
    //
    //while(1);
    // "void foo(void)"类型的函数指针。
    // +4因为第一个字是栈地址，第二个字才是复位向量的函数指针（注：不是函数地址）
    //(* ((void (*)(void))(MAP_FLASH_BASE_BYTE+4)) )();
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH,(uint32_t)MAP_APP_BASE_BYTE);
    int32_t pReset = *(int32_t *)(MAP_APP_BASE_BYTE+4);
    ((void (*)(void))(pReset))();
    //
    return 0;
}
