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

// control/control.c
// 2013-1-8 16:30:59
// js200300953

#include <math.h>
#include <bsp/time.h>
#include <bsp/motor.h>
#include <app/communication/interface.h>
#include <app/math/interface.h>
#include <app/attitude/interface.h>
#include <app/param/interface.h>
#include "control.h"
#include "pid.h"

enum
{
    CTRL_MODE_HALT = 1,      // 停机。
    CTRL_MODE_LOCK_THROTTLE, // 锁定油门模式。
    CTRL_MODE_LOCK_ATTITUDE, // 锁定目标姿态模式。
};

const uint32_t CTRL_PERIOD_MS = 10;

static struct
{
    struct
    {
        quaternion  baseAttitude;
        quaternion  target;
        float       baseThrottle;
        int32_t     isReady;
    }lockAttitude;
    float throttle[4];
    uint8_t mode;
}control;

void    ctrl_init(void);
void    ctrl_checkEvent(void);
void    ctrl_enterHaltMode(void);
void    ctrl_enterLockAttitudeMode(void);
void    ctrl_enterLockThrottleMode(void);
int32_t ctrl_lockAttitude_getReady(void);
int32_t ctrl_lockAttitude_control(const quaternion * c);
int32_t ctrl_lockAttitude_setBaseThrottle(float baseThrottle);
void    ctrl_lockThrottle_setThrottle(const float th[4]);
const float * ctrl_getCurrentThrottle(void);
//
static void ctrl_output(void);
static void ctrl_outputSame(float all_esc_throttle);
static void ctrl_outputByTargetAttitude(const quaternion * target,float baseThrottle);
static int32_t ctrl_lockAttitude_output(void);

void ctrl_init(void)
{
    control.mode = CTRL_MODE_HALT;
    pid_init();
    for(int32_t i=0;i<4;i++)
        control.throttle[i] = 0;
}

void ctrl_checkEvent(void)
{
    TIME_EXECUTE_PERIODICALLY_BEGIN(CTRL_PERIOD_MS)
    {
        // 如果倾斜太厉害（cos(45°)/2==0.3535），关闭电机。
        const quaternion * q = attitude_getAttitude();
        if(control.mode != CTRL_MODE_HALT
            && (1 - 2*q->x*q->x - 2*q->y*q->y < 0.3535))
        {
            cmd_returnMessage("倾斜停机");
            ctrl_enterHaltMode();
        }
        //
        switch(control.mode)
        {
            case CTRL_MODE_HALT :
                ctrl_outputSame(0);
                break;
            case CTRL_MODE_LOCK_ATTITUDE :
                ctrl_lockAttitude_output();
                break;
            case CTRL_MODE_LOCK_THROTTLE :
                break;
            //
        }
    } TIME_EXECUTE_PERIODICALLY_END();
}

void ctrl_output(void)
{
    motor_setForce(control.throttle);
}

void ctrl_outputSame(float esc_throttle)
{
    for(int i=0;i<4;i++)
        control.throttle[i] = esc_throttle;
    ctrl_output();
}

void ctrl_outputByTargetAttitude(const quaternion * target,float baseThrottle)
{
    //
    // 计算旋转差。
    const quaternion * cur = attitude_getAttitude();
    quaternion cur_rev;
    quaternion delta;
    cur_rev.w =   cur->w;
    cur_rev.x = - cur->x;
    cur_rev.y = - cur->y;
    cur_rev.z = - cur->z;
    quaternion_mult(&delta,&cur_rev,target);
    quaternion_normalize(&delta);
    //
    // 输入PID。
    float output[3] = {0,0,0};
    pid_iterate(&(delta.x),output);
    //
    // 计算油门，PD控制。
    control.throttle[0] = baseThrottle             - output[1] + output[2];
    control.throttle[1] = baseThrottle + output[0]             - output[2];
    control.throttle[2] = baseThrottle             + output[1] + output[2];
    control.throttle[3] = baseThrottle - output[0]             - output[2];
    //
    // 输出油门。
    ctrl_output();
}

void ctrl_enterHaltMode(void)
{
    control.mode = CTRL_MODE_HALT;
    ctrl_outputSame(0);
    //
    cmd_returnMessage(
        "\xE8\xBF\x9B\xE5\x85\xA5\xE5\x81"
        "\x9C\xE6\x9C\xBA\xE6\xA8\xA1\xE5\xBC\x8F"
        /*"进入停机模式"*/);
}

void ctrl_enterLockAttitudeMode(void)
{
    control.mode = CTRL_MODE_LOCK_ATTITUDE;
    control.lockAttitude.isReady = 0;
    //
    cmd_returnMessage(
        "\xE8\xBF\x9B\xE5\x85\xA5\xE9\x94"
        "\x81\xE5\xAE\x9A\xE5\xA7\xBF\xE6"
        "\x80\x81\xE6\xA8\xA1\xE5\xBC\x8F"
        /*"进入锁定姿态模式"*/);
}

void ctrl_enterLockThrottleMode(void)
{
    control.mode = CTRL_MODE_LOCK_THROTTLE;
    ctrl_outputSame(0);
    //
    cmd_returnMessage(
        "\xE8\xBF\x9B\xE5\x85\xA5\xE9\x94"
        "\x81\xE5\xAE\x9A\xE6\xB2\xB9\xE9"
        "\x97\xA8\xE6\xA8\xA1\xE5\xBC\x8F"
        /*"进入锁定油门模式"*/);
}

void ctrl_lockThrottle_setThrottle(const float th[4])
{
    if(control.mode != CTRL_MODE_LOCK_THROTTLE)
        return;
    //
    for(int i=0;i<4;i++)
        control.throttle[i] = th[i];
    ctrl_output();
}

int32_t ctrl_lockAttitude_output(void)
{
    if(! control.lockAttitude.isReady
        || control.lockAttitude.baseThrottle < 0.001)
    {
        ctrl_outputSame(0);
        return 0;
    }
    ctrl_outputByTargetAttitude(&(control.lockAttitude.target),
        control.lockAttitude.baseThrottle);
    //
    return 0;
}

int32_t ctrl_lockAttitude_getReady(void)
{
    if(control.mode != CTRL_MODE_LOCK_ATTITUDE)
        return -1;
    //
    control.lockAttitude.baseThrottle = 0;
    //
    // 计算目标姿态。
    quaternion yawCali = *attitude_getAttitude();
    yawCali.x = 0;
    yawCali.y = 0;
    //
    quaternion_mult(&(control.lockAttitude.baseAttitude),
                    &(yawCali),
                    &(param_getRamParam()->horizon));
    quaternion_normalize(&(control.lockAttitude.baseAttitude));
    control.lockAttitude.target = control.lockAttitude.baseAttitude;
    //
    control.lockAttitude.isReady = 1;
    //
    return 0;
}

int32_t ctrl_lockAttitude_control(const quaternion * c)
{
    if(control.mode != CTRL_MODE_LOCK_ATTITUDE
        || (! control.lockAttitude.isReady))
        return -1;
    //
    quaternion_mult(&(control.lockAttitude.target),
                    &(control.lockAttitude.baseAttitude),c);
    //
    ctrl_lockAttitude_output();
    return 0;
}

int32_t ctrl_lockAttitude_setBaseThrottle(float baseThrottle)
{
    if(control.mode != CTRL_MODE_LOCK_ATTITUDE
        || (! control.lockAttitude.isReady))
        return -1;
    //
    control.lockAttitude.baseThrottle = baseThrottle;
    ctrl_lockAttitude_output();
    return 0;
}

const float * ctrl_getCurrentThrottle(void)
{
    return control.throttle;
}
