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

#include "vector.h"
#include <math.h>

Vector::Vector()
{
}

void Vector::cross(const float left[], const float right[], float result[])
{
    result[0] = left[1]*right[2] - left[2]*right[1];
    result[1] = left[2]*right[0] - left[0]*right[2];
    result[2] = left[0]*right[1] - left[1]*right[0];
}

void Vector::normalize(float v[])
{
    float norm = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    norm = 1.0f / sqrtf(norm);
    //
    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}
