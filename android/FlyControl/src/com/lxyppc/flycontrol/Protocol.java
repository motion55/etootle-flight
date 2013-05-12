//     Copyright (c) 2013 js200300953@qq.com,lxyppc@163.com All rights reserved.
//         ========圆点博士微型四轴飞行器配套程序声明========
// 
// 圆点博士微型四轴飞行器配套程序包括上位机程序、下位机Bootloader和
//     下位机App，及它们的源代码，以下总称“程序”。
// 程序由js200300953,lxyppc编写。
// 程序仅为使用者提供参考，js200300953,lxyppc不对程序提供任何明示或暗含的担保，
//     不对在使用该程序中出现的意外或者损失负责，
//     也不对因使用该程序而引起的第三方索赔负责。
// 使用者可以以学习为目的修改和使用该程序，请勿以商业的目的使用该程序。
// 修改该程序时，必须保留原版权声明，并且不能修改原版权声明。
// 
// 更多资料见：
//     http://blog.sina.com.cn/js200300953
//     http://hi.baidu.com/lxyppc
//     http://www.etootle.com/
//     http://www.amobbs.com/thread-5504090-1-1.html
//     圆点博士微型四轴飞行器QQ群：276721324

package com.lxyppc.flycontrol;

public final class Protocol {
    static final int VERSION = 0;
    //
    static final int RETURN_ATTITUDE_QUATERNION         = 0x01;
    static final int RETURN_CALIBRATION_RAW_ACC_MAG     = 0x02;
    static final int PERIOD_MARK                        = 0x03;
    static final int RETURN_TRANSLATION                 = 0x04;
    static final int SET_ATTITUDE_QUATERNION            = 0x05;
    static final int GET_ATTITUDE_QUATERNION            = 0x06;
    static final int HEARTBEAT_SEND                     = 0x07;
    static final int HEARTBEAT_RETURN                   = 0x08;
    static final int GET_TRANSLATION                    = 0x09;
    static final int GET_ATTITUDE_ANGLE                 = 0x0A;
    static final int RETURN_ATTITUDE_ANGLE              = 0x0B;
    static final int LOCK_ATTITUDE_CURRENT_AS_TARGET    = 0x0C;
    static final int LOCK_ATTITUDE_ROTATE_WORLD_TIME    = 0x0D;
    static final int LOCK_ATTITUDE_SET_THROTTLE_BASE    = 0x0E;
    static final int SET_PARAMETER                      = 0x0F;
    static final int GET_PARAMETER                      = 0x10;
    static final int RETURN_PARAMETER                   = 0x11;
    static final int SET_CONTROL_MODE                   = 0x12;
    static final int LOCK_THROTTLE_SET_THROTTLE         = 0x13;
    static final int GET_VECTOR                         = 0x14;
    static final int RETURN_VECTOR                      = 0x15;
    static final int RETURN_OSC                         = 0x16;
    static final int LOCK_ATTITUDE_SET_TARGET           = 0x17;
    static final int GET_STATUS                         = 0x18;
    static final int RETURN_STATUS                      = 0x19;
    static final int RETURN_MESSAGE                     = 0x1A;
    static final int BOOTLOADER_CMD                     = 0x1B;
    static final int BOOTLOADER_STATUS                  = 0x1C;
    static final int PARAMETER                          = 0x1D;
    static final int LOCK_ATTITUDE                      = 0x1E;
    static final int POSITION_IN_CAMERA                 = 0x1F;
    static final int RAW_CONTROL_DATA					= 0x20;
    //
    final class ControlMode
    {
        static final int LOCK_THROTTLE = 0x01;
        static final int LOCK_ATTITUDE = 0x02;
        static final int HALT          = 0x03;
    }
    final class VectorType
    {
        static final int RAW_ACC        = 0x01;
        static final int CALIBRATED_ACC = 0x02;
        static final int RAW_GYR        = 0x03;
        static final int CALIBRATED_GYR = 0x04;
        static final int RAW_MAG        = 0x05;
        static final int CALIBRATED_MAG = 0x06;
    }
    final class Status
    {
        static final int BATERY_VOLTAGE   = 0x01;
        static final int CURRENT_THROTTLE = 0x02;
    }
    final class BootloaderCmd
    {
        static final int ENTER_BOOTLOADER   = 0x01;
        static final int BEGIN_REFRESH_APP  = 0x02;
        static final int APP_LENGTH_CRC     = 0x03;
        static final int WRITE_APP          = 0x04;
        static final int QUEST_REMAIN_BLOCK = 0x05;
        static final int ENTER_APP          = 0x06;
    }
    final class BootloaderStatus
    {
        static final int STAY_AT_BOOTLOADER   = 0x01;
        static final int READY_TO_REFRESH_APP = 0x02;
        static final int WRITING_APP          = 0x03;
        static final int BLOCK_REMAIN         = 0x04;
        static final int APP_CRC_ERROR        = 0x05;
        static final int WRITE_APP_SUCCEED    = 0x06;
        static final int RUNNING_APP          = 0x07;
    }
    final class Parameter
    {
        static final int GET_RAM        = 0x11;
        static final int RETURN_RAM     = 0x12;
        static final int SET_RAM        = 0x13;
        static final int GET_FLASH      = 0x21;
        static final int RETURN_FLASH   = 0x22;
        static final int WRITE_FLASH    = 0x23;
        static final int FLASH_VERIFY   = 0x24;
        static final int RESET_FLASH    = 0x25;
    }
    final class LockAttitude
    {
        static final int GET_READY  = 0x01;
        static final int READY      = 0x02;
        static final int NOT_READY  = 0x03;
        static final int CONTROL    = 0x04;
    }
    final class PositionInCamera
    {
        static final int PIXEL_UNIT = 0x01;
    }
}
