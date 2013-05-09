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

// bsp/mpu6050.c
// 2013-1-7 18:47:54
// js200300953

#include "mpu6050.h"
#include "i2c.h"
#include "time.h"
#include <app/param/interface.h>

#define MPU6050_ADDRESS 0xD0
#define MPU6050_REG_RAW_DATA 0x3B
#define MPU6050_REG_ACC 0x3B
#define MPU6050_REG_TEM 0x41
#define MPU6050_REG_GYR 0x43

static struct
{
    int16_t acc[3];
    int16_t tem;
    int16_t gyr[3];
}mpu6050_rawData;

int32_t mpu6050_init(void);
int32_t mpu6050_isDataReady(void);
int32_t mpu6050_read(void);
int32_t mpu6050_getRawAcc(int16_t acc[3]);
int32_t mpu6050_getRawGyr(int16_t gyr[3]);
int32_t mpu6050_getCalibratedAcc(float acc[3]);
int32_t mpu6050_getCalibratedGyr(float gyr[3]);

int32_t mpu6050_init(void)
{
    for(int i=0;i<sizeof(mpu6050_rawData);i++)
        ((uint8_t *)&mpu6050_rawData)[i] = 0;
    //
    // 检查ID。
    uint8_t id = 0;
    if(i2c_read(MPU6050_ADDRESS,0x75,&id,1) != 0)
        return MPU6050_FAILED;
    if(id != 0x68)
        return MPU6050_FAILED;
    //
    // 设置MPU6050的参数。
    const uint8_t param[][2] = 
    {
        // {寄存器地址,寄存器值},
        {0x6B,1     }, // 退出睡眠模式，设取样时钟为陀螺X轴。
        {0x19,4     }, // 取样时钟4分频，1k/4，取样率为25Hz。
        {0x1A,2     }, // 低通滤波，截止频率100Hz左右。
        {0x1B,3<<3  }, // 陀螺量程，2000dps。
        {0x1C,2<<3  }, // 加速度计量程，8g。
        {0x37,0x32  }, // 中断信号为高电平，推挽输出，直到有读取操作才消失，直通辅助I2C。
        {0x38,1     }, // 使用“数据准备好”中断。
        {0x6A,0x00  }, // 不使用辅助I2C。
    };
    for(int i=0;i<sizeof(param)/2;i++)
    {
        if(i2c_write(MPU6050_ADDRESS,param[i][0],&(param[i][1]),1) != 0)
            return MPU6050_FAILED;
        //
        uint8_t check = 0;
        if(i2c_read(MPU6050_ADDRESS,param[i][0],&check,1) != 0)
            return MPU6050_FAILED;
        if(check != param[i][1])
            return MPU6050_FAILED;
    }
    //
    return MPU6050_SUCCEED;
}

int32_t mpu6050_dataIsReady(void)
{
    static uint32_t target = 0;
    uint32_t now = time_nowMs();
    if(target <= now)
    {
        target = now + 3;//333Hz
        //
        return 1;
    }
    return 0;
}

int32_t mpu6050_read(void)
{
    if(i2c_read(MPU6050_ADDRESS,MPU6050_REG_RAW_DATA,&mpu6050_rawData
        ,sizeof(mpu6050_rawData)) != 0)
        return MPU6050_FAILED;
    //
    // 大端转小端。
    uint8_t * p = (uint8_t *)&mpu6050_rawData;
    for(int i=0;i<sizeof(mpu6050_rawData)/2;i++)
    {
        uint8_t tmp = p[2*i];
        p[2*i] = p[2*i + 1];
        p[2*i + 1] = tmp;
    }
    //
    return MPU6050_SUCCEED;
}

// acc : LSB
int32_t mpu6050_getRawAcc(int16_t acc[3])
{
    for(int i=0;i<3;i++)
        acc[i] = mpu6050_rawData.acc[i];
    //
    return MPU6050_SUCCEED;
}

//gyr : LSB
int32_t mpu6050_getRawGyr(int16_t gyr[3])
{
    for(int i=0;i<3;i++)
        gyr[i] = mpu6050_rawData.gyr[i];
    //
    return MPU6050_SUCCEED;
}

// acc : m/s²
int32_t mpu6050_getCalibratedAcc(float acc[3])
{
    const param_t *p = param_getRamParam();
    //
    for(int i=0;i<3;i++)
        acc[i] = (p->acc_offset[i] + mpu6050_rawData.acc[i]) * p->acc_gain[i];
    //
    return MPU6050_SUCCEED;
}

// gyr : rad/s
int32_t mpu6050_getCalibratedGyr(float gyr[3])
{
    const param_t *p = param_getRamParam();
    //
    for(int i=0;i<3;i++)
        gyr[i] = (p->gyr_offset[i] + mpu6050_rawData.gyr[i]) * p->gyr_gain[i];
    //
    return MPU6050_SUCCEED;
}
