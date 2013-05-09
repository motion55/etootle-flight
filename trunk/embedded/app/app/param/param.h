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

// app/param/param.h
// 2013-4-8 9:07:11
// js200300953

#ifndef __FILE_APP_PARAM_PARAM_H__
#define __FILE_APP_PARAM_PARAM_H__

#include <stdint.h>
#include <app/math/interface.h>

// 定义参数的结构体，成员位置不能随便改。
typedef struct
{
    // 参数信息，pos=0。
    uint8_t version; // 参数结构的版本。
    uint8_t length;  // 参数总长度，单位：字，即字节数/4。
    uint16_t reserve_zero_0;
    //
    // 传感器校正参数，pos=[1,18]。
    float acc_offset[3];
    float acc_gain[3];
    float gyr_offset[3];
    float gyr_gain[3];
    float mag_offset[3];
    float mag_gain[3];
    //
    // 四元数表示的水平面，pos=[19,22]。
    quaternion horizon;
    //
    // 控制参数，[xyz][pid]，pos=[23,31]。
    struct
    {
        float p,i,d; // 顺序严格按P、I、D。
    }xyz_pid[3];
    //
    uint32_t crc;
} param_t;

int32_t         param_init(void);
const param_t * param_getRamParam(void);
param_t *       param_setRamParam(void);
const param_t * param_getFlashParam(void);
int32_t         param_writeToFlash(void);
int32_t         param_resetFlash(void);

#endif // __FILE_APP_PARAM_PARAM_H__
