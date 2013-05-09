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

// app/control/pid.c
// 2013-3-2 12:58:36
// js200300953

#include <app/param/interface.h>
#include "pid.h"

static float pid_pre[3] = {0,0,0};
static float pid_accumulate[3] = {0,0,0};

int32_t pid_init(void)
{
    return 0;
}

int32_t pid_iterate(const float input[3],float output[3])
{
    const param_t *p = param_getRamParam();
    //
    for(int i=0;i<3;i++)
    {
        float p_o = input[i] * p->xyz_pid[i].p;
        pid_accumulate[i] += input[i] * p->xyz_pid[i].i;
        float i_o = pid_accumulate[i];
        if(i_o < -0.1)
            i_o = -0.1;
        if(i_o > 0.1)
            i_o = 0.1;
        float d_o = (input[i] - pid_pre[i]) * p->xyz_pid[i].d;
        //
        output[i] = p_o + i_o + d_o;
        //
        pid_pre[i] = input[i];
    }
    //
    return 0;
}
