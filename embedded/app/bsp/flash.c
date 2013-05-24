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

// bsp/flash.c
// 2013-4-11 10:21:02
// js200300953

#include <cmsis/stm32f10x.h>
#include "flash.h"

int32_t flash_init(void);
int32_t flash_erase(const uint32_t * page);
int32_t flash_write(const uint32_t * target,const uint32_t * source,int32_t length);

int32_t flash_init(void)
{
#ifndef NO_BOOTLOADER
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,(uint32_t)0x4000);
#else
#warning vector not set when compile without bootloader
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,(uint32_t)0x0000);
#endif
    return 0;
}

int32_t flash_erase(const uint32_t * page)
{
    int32_t rt = FLASH_COMPLETE;
    //
    FLASH_Unlock();
    rt = FLASH_ErasePage((uint32_t)page);
    FLASH_Lock();
    //
    if(rt != FLASH_COMPLETE)
        return -1;
    return 0;
}

int32_t flash_write(const uint32_t * target,const uint32_t * source,int32_t length)
{
    int32_t ok = 1;
    //
    FLASH_Unlock();
    for(int32_t i=0;i<length;i++)
    {
        if(FLASH_ProgramWord((uint32_t)(target+i),source[i]) != FLASH_COMPLETE)
        {
            ok = 0;
            break;
        }
    }
    FLASH_Lock();
    //
    if(! ok)
        return -1;
    return 0;
}
