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

// app/refresher/refresher.h
// 2013-2-21 21:04:21
// js200300953

#ifndef __FILE_APP_REFRESHER_REFRESHER_H__
#define __FILE_APP_REFRESHER_REFRESHER_H__

#include <stdint.h>

int32_t refresher_init(void);
int32_t refresher_checkApp(void);
int32_t refresher_setAppSizeCrc(uint32_t size_block,uint32_t crc);
int32_t refresher_eraseApp(void);
int32_t refresher_write(const uint32_t *data,int32_t length,int32_t blockIndexOfFirst);
int32_t refresher_isWriteComplete(void);
int32_t refresher_getRemainBlock(uint8_t * buffer,int32_t bufferLength);
int32_t refresher_getWrittenAmount_block(void);
int32_t refresher_jumpToApp(void);

#endif // __FILE_APP_REFRESHER_REFRESHER_H__
