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

// bsp/hmc5883.c
// 2012-12-9 14:46:53
// js200300953

#include "hmc5883.h"
#include "i2c.h"
#include "time.h"
#include <app/param/interface.h>

#define HMC5883_ADDRESS 0x3C
#define HMC5883_REG_MAG 3
#define HMC5883_REG_STATUS 9
#define HMC5883_REG_STATUS_DRDY_MASK 0x01
#define HMC5883_REG_SETTING 0

static int16_t hmc5883_rawMag[3];

int32_t hmc5883_init(void);
int32_t hmc5883_isDataReady(void);
int32_t hmc5883_read(void);
int32_t hmc5883_getRawMag(int16_t mag[3]);
int32_t hmc5883_getCalibratedMag(float mag[3]);

int32_t hmc5883_init(void)
{
    for(int i=0;i<3;i++)
        hmc5883_rawMag[i] = 0;
    //
    // 检查ID。
    uint8_t id[3];
    if(i2c_read(HMC5883_ADDRESS,10,id,3))
        return HMC5883_FAILED;
    if(id[0] != 0x48 || id[1] != 0x34 || id[2] != 0x33)
        return HMC5883_FAILED;
    //
    // 设置HMC5883的参数。
    // 设置MPU6050的参数。
    const uint8_t param[][2] = 
    {
        // {寄存器地址,寄存器值},
        {0x00,0x78  }, // RegA，滑动平均窗口：8，采样率：75Hz。
        {0x01,1<<6  }, // RegB，量程：+-1.9Ga。
        {0x02,0x00  }, // mode，连续测量模式。
    };
    int8_t rt = 0;
    for(int i=0;i<sizeof(param)/2;i++)
    {
        if(i2c_write(HMC5883_ADDRESS,param[i][0],&param[i][1],1))
            return HMC5883_FAILED;
        //
        uint8_t check = 0;
        if(i2c_read(HMC5883_ADDRESS,param[i][0],&check,1))
            return HMC5883_FAILED;
        if(check != param[i][1])
            rt |= (1<<i);
    }
    //
    return rt;
}

int32_t hmc5883_isDataReady(void)
{
    static uint32_t target = 0;
    uint32_t now = time_nowMs();
    if(target <= now)
    {
        target = now + 20;//50Hz
        //
        return 1;
    }
    return 0;
}

int32_t hmc5883_read(void)
{
    if(i2c_read(HMC5883_ADDRESS,HMC5883_REG_MAG,
        hmc5883_rawMag,3*2))
        return HMC5883_FAILED;
    //
    // YZ调换。
    int16_t tmp = hmc5883_rawMag[1];
    hmc5883_rawMag[1] = hmc5883_rawMag[2];
    hmc5883_rawMag[2] = tmp;
    //
    // 调整字节顺序到小端。
    uint8_t * p = (uint8_t *)hmc5883_rawMag;
    for(int i=0;i<3;i++)
    {
        uint8_t tmp = p[2*i];
        p[2*i] = p[2*i + 1];
        p[2*i + 1] = tmp;
    }
    //
    return HMC5883_SUCCEED;
}

// LSB
int32_t hmc5883_getRawMag(int16_t mag[3])
{
    for(int i=0;i<3;i++)
        mag[i] = hmc5883_rawMag[i];
    //
    return HMC5883_SUCCEED;
}

// acc : m/s²
// gyr : rad/s
int32_t hmc5883_getCalibratedMag(float mag[3])
{
    const param_t *p = param_getRamParam();
    //
    float tmp[3];
    for(int i=0;i<3;i++)
        tmp[i] = (p->mag_offset[i] + hmc5883_rawMag[i]) * p->mag_gain[i];
    //
    // 绕Z旋转-45°（安装问题）。
    mag[0] = 0.7071067/* 根号2除以2 */ * ( tmp[0] + tmp[1]);
    mag[1] = 0.7071067/* 根号2除以2 */ * (-tmp[0] + tmp[1]);
    mag[2] = tmp[2];
    //
    return HMC5883_SUCCEED;
}
