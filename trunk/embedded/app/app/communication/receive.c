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

// app/communication/receive.c
// 2013-1-8 14:01:13
// js200300953

#include "bsp/rf.h"
#include "bsp/led.h"
#include "app/math/interface.h"
#include "receive.h"
#include "protocol.h"
#include "cmd.h"

void receive_init(void)
{
}

void receive_checkEvent(void)
{
    rf_checkEvent();
}

void rf_handleReceivedData(const uint8_t * packet,int32_t length)
{
    if(length < 4)
        return;
    if(math_crc16(0,packet,length) != 0)
        return;
    if(packet[0] != PROTOCOL_VERSION)
        return;
    //
    led_reverse();
    //
    const uint8_t * param = packet + 2;
    int32_t param_length = length - 4;
    switch(packet[1])
    {
    case PROTOCOL_GET_ATTITUDE_QUATERNION :
        {
            if(param_length != 0)
                break;
            //
            cmd_getAttitude();
            break;
        }
    case PROTOCOL_GET_VECTOR :
        {
            if(param_length != 1)
                break;
            //
            cmd_getVector(param[0]);
            break;
        }
    case PROTOCOL_SET_CONTROL_MODE :
        {
            if(param_length != 1)
                break;
            //
            cmd_setControlMode(param[0]);
            break;
        }
    case PROTOCOL_LOCK_THROTTLE_SET_THROTTLE :
        {
            if(param_length != 4*4)
                break;
            //
            float th[4];
            for(int i=0;i<4*4;i++)
                ((uint8_t *)th)[i] = param[i];
            //
            cmd_lockThrottle_setThrottle(th);
            //
            break;
        }
    case PROTOCOL_GET_STATUS :
        {
            if(param_length < 1)
                break;
            //
            cmd_getStatus(param,param_length);
            break;
        }
    case PROTOCOL_BOOTLOADER_CMD :
        {
            if(param_length < 1)
                break;
            cmd_bootloaderCmd(param[0],param+1,param_length);
        }
        break;
    case PROTOCOL_PARAMETER :
        cmd_parameter(param,param_length);
        break;
    case PROTOCOL_LOCK_ATTITUDE:
        cmd_lockAttitude(param,param_length);
        break;
    }
}
