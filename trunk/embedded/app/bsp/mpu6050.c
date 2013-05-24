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
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

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

int32_t mpu6050_DMPdataIsReady(void)
{
    static uint32_t target = 0;
    uint32_t now = time_nowMs();
    if(target <= now)
    {
        target = now + 5;// 1000/5 = 200 Hz
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

#define  SAMPLE_RATE  100   // 100Hz 

static signed char gyro_orientation[9] = {1,  0,  0,
                                           0, 1,  0,
                                           0,  0,  1};

static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static struct int_param_s int_param = {
    .pin = 0,
    .cb = 0,
    .arg = 0,
    .spi = 0,
};

static void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result & 0x1) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
		//printf( "MPU self test success\n\r" );
    }
	else {
		//printf( "MPU self test failed\n\r" );
    }
}

int32_t mpu6050_dmp_init(void)
{
    int result;
    unsigned char accel_fsr;
    unsigned short gyro_rate, gyro_fsr;
    unsigned short dmp_features;
    result = mpu_init(&int_param);
    /* If you're not using an MPU9150 AND you're not using DMP features, this
     * function will place all slaves on the primary bus.
     * mpu_set_bypass(1);
     */

    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    /* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(SAMPLE_RATE);
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);

    /* Initialize HAL state variables. */
    //memset(&hal, 0, sizeof (hal));
    //hal.sensors = ACCEL_ON | GYRO_ON;
    //hal.report = PRINT_QUAT;

    /* To initialize the DMP:
     * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
     *    inv_mpu_dmp_motion_driver.h into the MPU memory.
     * 2. Push the gyro and accel orientation matrix to the DMP.
     * 3. Register gesture callbacks. Don't worry, these callbacks won't be
     *    executed unless the corresponding feature is enabled.
     * 4. Call dmp_enable_feature(mask) to enable different features.
     * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
     * 6. Call any feature-specific control functions.
     *
     * To enable the DMP, just call mpu_set_dmp_state(1). This function can
     * be called repeatedly to enable and disable the DMP at runtime.
     *
     * The following is a short summary of the features supported in the DMP
     * image provided in inv_mpu_dmp_motion_driver.c:
     * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
     * 200Hz. Integrating the gyro data at higher rates reduces numerical
     * errors (compared to integration on the MCU at a lower sampling rate).
     * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
     * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
     * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
     * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
     * an event at the four orientations where the screen should rotate.
     * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
     * no motion.
     * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
     * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
     * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
     * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
     */
    result = dmp_load_motion_driver_firmware();
#ifdef DELUG_USE_TERMINAL
    if (0 == result)
        printUART(">>> Load Firmware Successful!\n");
#endif
    dmp_set_orientation(
            inv_orientation_matrix_to_scalar(gyro_orientation));
    //dmp_register_tap_cb(tap_cb);
    //dmp_register_android_orient_cb(android_orient_cb);
    dmp_features = DMP_FEATURE_6X_LP_QUAT /*| DMP_FEATURE_TAP |
            DMP_FEATURE_ANDROID_ORIENT */| DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
            DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(dmp_features);
    dmp_set_fifo_rate(SAMPLE_RATE);
    mpu_set_dmp_state(1);
    
    run_self_test();
    
    // read dmp data
    //dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more );
    
    return 0;
}

