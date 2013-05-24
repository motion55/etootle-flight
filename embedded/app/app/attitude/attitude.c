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

// app/attitude/attitude.c
// 2012-11-7 18:12:59
// js200300953

#include "bsp/time.h"
#include "app/math/interface.h"
#include "attitude.h"
#include "accfilter.h"
#include "gyrfilter.h"
#include "magfilter.h"
#include "mix.h"

static quaternion attitude;

void attitude_init(void);
const quaternion * attitude_getAttitude(void);
void attitude_inputAcc(const float acc[3]);
void attitude_inputGyr(const float gyr[3]);
void attitude_inputMag(const float mag[3]);
void attitude_mixAccGyrMag(void);

void attitude_init(void)
{
    accfilter_init();
    gyrfilter_init();
    magfilter_init();
    //
    quaternion_loadIdentity(&attitude);
}

// 获取姿态，只读。
const quaternion * attitude_getAttitude(void)
{
    return &attitude;
}

// acc : 加速度测量值，m/s²。
void attitude_inputAcc(const float acc[3])
{
    accfilter_input(acc);
}

// gyr : 角速度测量值，rad/s。
void attitude_inputGyr(const float gyr[3])
{
    gyrfilter_input(gyr);
}

// mag : 磁场方向测量值，标定到长度为1。
void attitude_inputMag(const float mag[3])
{
    magfilter_input(mag);
}

void attitude_mixGyrAccMag(void)
{
    //
    // 计算积分间隔，判断是否合理。
    static uint64_t time_pre_us = 0;
    uint64_t time_now_us = time_nowUs();
    int32_t time_interval_us = time_now_us - time_pre_us;
    time_pre_us = time_now_us;
    if(time_interval_us > 1000*1000) // 超过1秒就判为异常，丢弃。
        return;
    float time_interval_s = time_interval_us * (1.0f/1e6f);
    //
    //mix_gyrAccMag_crossMethod(&attitude,gyrfilter_current(),accfilter_getCurrent(),magfilter_getCurrent(),time_interval_s);
    mix_gyrAcc_crossMethod(&attitude,gyrfilter_current(),accfilter_getCurrent(),time_interval_s);
}

void attitude_mixDMPResult(float w, float x, float y, float z)
{
    attitude.w = w;
    attitude.x = x;
    attitude.y = y;
    attitude.z = z;
    quaternion_normalize(&attitude);
}
