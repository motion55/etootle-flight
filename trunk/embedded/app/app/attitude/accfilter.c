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

// app/attitude/accfilter.c
// 2013-1-8 15:51:54
// js200300953

#include "accfilter.h"

#define ACCFILTER_AVG_NUM 16

static float accfilter_history[ACCFILTER_AVG_NUM][3];
static float accfilter_current[3];
static uint8_t accfilter_index;

void accfilter_init(void);
void accfilter_input(const float val[3]);
const float * accfilter_getCurrent(void);

void accfilter_init(void)
{
    for(int i=0;i<ACCFILTER_AVG_NUM;i++)
    {
        for(int j=0;j<3;j++)
            accfilter_history[i][j] = 0;
    }
    for(int i=0;i<3;i++)
        accfilter_current[i] = 0;
    accfilter_index = 0;
}

void accfilter_input(const float val[3])
{
    for(int i=0;i<3;i++)
        accfilter_history[accfilter_index][i] = val[i];
    accfilter_index ++;
    if(accfilter_index >= ACCFILTER_AVG_NUM)
        accfilter_index = 0;
    //
    float sum[3] = {0,0,0};
    for(int i=0;i<ACCFILTER_AVG_NUM;i++)
    {
        sum[0] += accfilter_history[i][0];
        sum[1] += accfilter_history[i][1];
        sum[2] += accfilter_history[i][2];
    }
    //
    for(int i=0;i<3;i++)
        accfilter_current[i] = sum[i] * (1.0f/ACCFILTER_AVG_NUM);
    //
}

const float * accfilter_getCurrent(void)
{
    return accfilter_current;
}
