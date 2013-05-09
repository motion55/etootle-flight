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

// app/sensor/sensor.h
// 2012-11-7 13:43:43
// js200300953

#include "bsp/mpu6050.h"
#include "bsp/hmc5883.h"
#include "app/attitude/interface.h"
#include "sensor.h"

void sensor_init(void);
void sensor_checkEvent(void);
void sensor_getRawAcc(int16_t acc[3]);
void sensor_getRawGyr(int16_t gyr[3]);
void sensor_getRawMag(int16_t mag[3]);
void sensor_getCalibratedAcc(float acc[3]);
void sensor_getCalibratedGyr(float gyr[3]);
void sensor_getCalibratedMag(float mag[3]);

void sensor_init(void)
{
}

void sensor_checkEvent(void)
{
    if(mpu6050_dataIsReady())
    {
        float acc[3],gyr[3];
        if(mpu6050_read() != 0)
            return;
        mpu6050_getCalibratedAcc(acc);
        mpu6050_getCalibratedGyr(gyr);
        //
        attitude_inputAcc(acc);
        attitude_inputGyr(gyr);
        //
        attitude_mixGyrAccMag();
    }
    //
    /*
    if(hmc5883_isDataReady())
    {
        float mag[3];
        if(hmc5883_read() != 0)
            return;
        hmc5883_getCalibratedMag(mag);
        //
        attitude_inputMag(mag);
    }
    */
}

void sensor_getRawAcc(int16_t acc[3])
{
    mpu6050_getRawAcc(acc);
}

void sensor_getRawGyr(int16_t gyr[3])
{
    mpu6050_getRawGyr(gyr);
}

void sensor_getRawMag(int16_t mag[3])
{
    hmc5883_getRawMag(mag);
}

void sensor_getCalibratedAcc(float acc[3])
{
    mpu6050_getCalibratedAcc(acc);
}

void sensor_getCalibratedGyr(float gyr[3])
{
    mpu6050_getCalibratedGyr(gyr);
}

void sensor_getCalibratedMag(float mag[3])
{
    hmc5883_getCalibratedMag(mag);
}
