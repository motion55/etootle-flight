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

// app/communication/cmd.c
// 2013-1-8 14:23:01
// js200300953

#include <stdint.h>
#include "cmd.h"
#include "protocol.h"
#include "transmit.h"
#include "app/sensor/interface.h"
#include "app/attitude/interface.h"
#include "app/control/interface.h"
#include "app/status/interface.h"
#include "app/param/interface.h"
#include <stm32f10x.h>

void    cmd_getVector(uint8_t type);
void    cmd_getAttitude(void);
void    cmd_setControlMode(uint8_t mode);
int32_t cmd_returnMessage(const char * msg);
void    cmd_bootloaderCmd(uint8_t cmd,const uint8_t param[],int32_t param_length);
void    cmd_returnAppInfo(void);
void    cmd_parameter(const uint8_t param[],int32_t param_length);
int32_t cmd_lockAttitude(const uint8_t param[],int32_t param_length);
//
static void cmd_returnParameter(uint8_t code,uint8_t pos,uint8_t len,const int32_t param[]);

void cmd_getVector(uint8_t type)
{
    if(transmit_packetBegin(PROTOCOL_RETURN_VECTOR) != 0)
        return;
    //
    float buffer[3];
    const void * vector = 0;
    //
    switch(type)
    {
        case PROTOCOL_VECTOR_TYPE_RAW_ACC:
        {
            int16_t raw[3];
            sensor_getRawAcc(raw);
            for(int i=0;i<3;i++)
                buffer[i] = raw[i];
            vector = buffer;
            //
            break;
        }
        case PROTOCOL_VECTOR_TYPE_RAW_GYR:
        {
            int16_t raw[3];
            sensor_getRawGyr(raw);
            for(int i=0;i<3;i++)
                buffer[i] = raw[i];
            vector = buffer;
            //
            break;
        }
        case PROTOCOL_VECTOR_TYPE_RAW_MAG:
        {
            int16_t raw[3];
            sensor_getRawMag(raw);
            for(int i=0;i<3;i++)
                buffer[i] = raw[i];
            vector = buffer;
            //
            break;
        }
        case PROTOCOL_VECTOR_TYPE_CALIBRATED_ACC :
        {
            sensor_getCalibratedAcc(buffer);
            vector = buffer;
            break;
        }
        case PROTOCOL_VECTOR_TYPE_CALIBRATED_GYR :
        {
            sensor_getCalibratedGyr(buffer);
            vector = buffer;
            break;
        }
        case PROTOCOL_VECTOR_TYPE_CALIBRATED_MAG :
        {
            sensor_getCalibratedMag(buffer);
            vector = buffer;
            break;
        }
    }
    //
    if(vector != 0)
    {
        transmit_packetContent(&type,1);
        transmit_packetContent(vector,3*4);
        transmit_packetEnd();
    }
}

void cmd_getAttitude(void)
{
    if(transmit_packetBegin(PROTOCOL_RETURN_ATTITUDE_QUATERNION) != 0)
        return;
    transmit_packetContent(attitude_getAttitude(),4*4);
    transmit_packetEnd();
}

void cmd_setControlMode(uint8_t mode)
{
    switch(mode)
    {
        case PROTOCOL_SET_CONTROL_MODE_HALT :
        {
            ctrl_enterHaltMode();
            break;
        }
        case PROTOCOL_SET_CONTROL_MODE_LOCK_THROTTLE :
        {
            ctrl_enterLockThrottleMode();
            break;
        }
        case PROTOCOL_SET_CONTROL_MODE_LOCK_ATTITUDE :
        {
            ctrl_enterLockAttitudeMode();
            break;
        }
    }
}

void cmd_getStatus(const uint8_t which[],int32_t amount)
{
    for(int i=0;i<amount;i++)
    {
        switch(which[i])
        {
            case PROTOCOL_STATUS_BATERY_VOLTAGE :
            {
                if(transmit_packetBegin(PROTOCOL_RETURN_STATUS) != 0)
                    return;
                //
                float v = status_getBateryVoltage();
                transmit_packetContent(&which[i],1);
                transmit_packetContent(&v,sizeof(v));
                transmit_packetEnd();
                break;
            }
            case PROTOCOL_STATUS_CURRENT_THROTTLE :
            {
                if(transmit_packetBegin(PROTOCOL_RETURN_STATUS) != 0)
                    return;
                //
                transmit_packetContent(&which[i],1);
                transmit_packetContent(status_getCurrentThrottle(),4*sizeof(float));
                transmit_packetEnd();
                break;
            }
        }
    }
}

int32_t cmd_returnMessage(const char * msg)
{
    if(transmit_packetBegin(PROTOCOL_RETURN_MESSAGE) != 0)
        return 1;
    //
    // 确定发送数据的长度，取字符串长度和最大包长的最小的一个。
    int32_t len = 0;
    while(len < TRANSMIT_CONTENT_LENGTH_MAX
          && msg[len] != '\0')
        len ++;
    //
    if(transmit_packetContent(msg,len) != 0)
        return 1;
    if(transmit_packetEnd() != 0)
        return 1;
    //
    return 0;
}

void cmd_bootloaderCmd(uint8_t cmd,const uint8_t param[],int32_t param_length)
{
    switch(cmd)
    {
        case PROTOCOL_BOOTLOADER_CMD_ENTER_BOOTLOADER :
        {
            // 请求进入bootloader。
            int32_t pReset = *(int32_t *)(0x08000000+4);
            __disable_irq();
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SPI1, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SPI1, DISABLE);
            __set_MSP(*(int32_t *)(0x08000000));
            ((void (*)(void))(pReset))();
        }
        break;
        case PROTOCOL_BOOTLOADER_CMD_ENTER_APP :
        {
            // 如果请求进入app，就返回“已经进入app”状态。
            if(transmit_packetBegin(PROTOCOL_BOOTLOADER_STATUS) != 0)
                return;
            //
            uint8_t status = PROTOCOL_BOOTLOADER_STATUS_RUNNING_APP;
            transmit_packetContent(&status,1);
            transmit_packetEnd();
        }
        break;
    }
}

void cmd_returnAppInfo(void)
{
    cmd_returnMessage("App v1.0");
    //cmd_returnMessage(__DATE__);
    //cmd_returnMessage(__TIME__);
}

void cmd_parameter(const uint8_t param[],int32_t param_length)
{
    //
    // 参数至少需要一个字节。
    if(param_length < 1)
        return;
    //
    int32_t * param_ram = (int32_t *)param_getRamParam();
    int32_t * param_flash = (int32_t *)param_getFlashParam();
    switch(param[0])
    {
    case PROTOCOL_PARAMETER_GET_RAM     :
        {
            // |pos:8|len:8|
            if(param_length < 2)
                return;
            //
            uint8_t pos = param[1];
            if(pos < 1 || pos >= (sizeof(param_t)/4))
                return;
            uint8_t len = param[2];
            if(len < 1 || pos+len > (sizeof(param_t)/4))
                return;
            //
            cmd_returnParameter(PROTOCOL_PARAMETER_RETURN_RAM,pos,len,param_ram+pos);
        }
        break;
    case PROTOCOL_PARAMETER_SET_RAM    :
        {
            // |pos:8|value:32*n|
            if(param_length < 1+4)
                return;
            //
            uint8_t pos = param[1];
            if(pos < 1 || pos >= (sizeof(param_t)/4))
                return;
            uint8_t len = (param_length-1)/4;
            if(len < 1 || pos+len > (sizeof(param_t)/4))
                return;
            //
            int32_t len_b = len * 4; // 变为字节单位。
            int32_t pos_b = pos * 4; // 变为字节单位。
            for(int i=0;i<len_b;i++)
            {
                ((uint8_t *)param_ram)[pos_b+i] = param[2+i];
            }
            //
            cmd_returnParameter(PROTOCOL_PARAMETER_RETURN_RAM,pos,len,param_ram+pos);
        }
        break;
    case PROTOCOL_PARAMETER_GET_FLASH   :
        {
            // |pos:8|len:8|
            if(param_length < 2)
                return;
            //
            uint8_t pos = param[1];
            if(pos < 1 || pos >= (sizeof(param_t)/4))
                return;
            uint8_t len = param[2];
            if(len < 1 || pos+len > (sizeof(param_t)/4))
                return;
            //
            cmd_returnParameter(PROTOCOL_PARAMETER_RETURN_FLASH,pos,len,param_flash+pos);
        }
        break;
    case PROTOCOL_PARAMETER_WRITE_FLASH  :
        //
        // |没有附加参数|
        param_writeToFlash();
        break;
    case PROTOCOL_PARAMETER_RESET_FLASH  :
        //
        // |没有附加参数|
        param_resetFlash();
        break;
    }
}

void cmd_returnParameter(uint8_t code,uint8_t pos,uint8_t len,const int32_t param[])
{
    if(transmit_packetBegin(PROTOCOL_PARAMETER) != 0)
        return;
    //
    if(transmit_packetContent(&code,1) != 0)
        return;
    if(transmit_packetContent(&pos,1) != 0)
        return;
    if(transmit_packetContent(param,len*4) != 0)
        return;
    if(transmit_packetEnd() != 0)
        return;
    //
}

int32_t cmd_lockAttitude(const uint8_t param[],int32_t param_length)
{
    if(param_length < 1)
        return -1;
    //
    switch(param[0])
    {
    case PROTOCOL_LOCK_ATTITUDE_GET_READY :
        {
            uint8_t cmd = PROTOCOL_LOCK_ATTITUDE_READY;
            if(ctrl_lockAttitude_getReady() == 0)
            {
                cmd = PROTOCOL_LOCK_ATTITUDE_READY;
                cmd_returnMessage("准备好起飞了");// 源文件要用UTF8编码。
            }
            else
                cmd = PROTOCOL_LOCK_ATTITUDE_NOT_READY;
            //
            if(transmit_packetBegin(PROTOCOL_LOCK_ATTITUDE) != 0)
                return -2;
            //
            if(transmit_packetContent(&cmd,1) != 0)
                return -2;
            if(transmit_packetEnd() != 0)
                return -2;
            break;
        }
    case PROTOCOL_LOCK_ATTITUDE_CONTROL :
        {
            if(param_length != (1+4+4*4))
                break;
            //
            float th;
            for(int i=0;i<4;i++)
                ((uint8_t *)&th)[i] = param[1+i];
            //
            quaternion target;
            for(int i=0;i<4*4;i++)
                ((uint8_t *)&target)[i] = param[1+4+i];
            //
            if(ctrl_lockAttitude_setBaseThrottle(th) != 0
                || ctrl_lockAttitude_control(&target) != 0)
            {
            }
            //
            break;
        }
    }
    //
    return 0;
}
