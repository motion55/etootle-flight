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

#include "quaternion.h"
#include <math.h>

Quaternion::Quaternion()
{
    loadIdentity();
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    m_value[0] = w;
    m_value[1] = x;
    m_value[2] = y;
    m_value[3] = z;
}

Quaternion::Quaternion(float yaw, float pitch, float roll)
{
    fromAngle(yaw,pitch,roll);
}

Quaternion::Quaternion(const float val[])
{
    assign(val);
}

// 加载单位四元数。
void Quaternion::loadIdentity(void)
{
    m_value[0] = 1;
    m_value[1] = m_value[2] = m_value[3] = 0;
}

// 转4*4矩阵。
void Quaternion::toMatrix(float mat[16])
{
    float x2  = m_value[1] * 2;
    float y2  = m_value[2] * 2;
    float z2  = m_value[3] * 2;
    float wx2 = m_value[0] * x2;
    float wy2 = m_value[0] * y2;
    float wz2 = m_value[0] * z2;
    float xx2 = m_value[1] * x2;
    float yy2 = m_value[2] * y2;
    float zz2 = m_value[3] * z2;
    float xy2 = m_value[1] * y2;
    float yz2 = m_value[2] * z2;
    float xz2 = m_value[3] * x2;
    //
    mat[0] = 1 - yy2 - zz2; mat[1] = xy2 - wz2;     mat[2] = xz2 + wy2;      mat[3] = 0;
    mat[4] = xy2 + wz2;     mat[5] = 1 - xx2 - zz2; mat[6] = yz2 - wx2;      mat[7] = 0;
    mat[8] = xz2 - wy2;     mat[9] = yz2 + wx2;     mat[10] = 1 - xx2 - yy2; mat[11] = 0;
    mat[12] = 0;            mat[13] = 0;            mat[14] = 0;             mat[15] = 1;
}

// 单位化。
void Quaternion::nomalize()
{
    //
    // 计算平方和。
    float norm = 0.0f;
    for(int i=0;i<4;i++)
        norm += m_value[i] * m_value[i];
    //
    // 判断容差。
    norm = fabs(norm);
    if(norm < ZERO)
    {
        loadIdentity();
        return;
    }
    //
    // 单位化。
    norm = 1 / sqrtf(norm);
    for(int i=0;i<4;i++)
        m_value[i] *= norm;
}

/*
 * 向量旋转 => 四元数旋转。
 * 参数
 *   vector : 旋转向量，输入。方向为旋转轴，长度为旋转角度，角度单位：弧度。
 *   result : 存放结果的四元数，输出。*/
void Quaternion::fromVectorRotation(const float vector[3])
{
    /*
     * 取向量的长度。 */
    float norm = fabsf(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
    if(norm < ZERO)
    {
        loadIdentity();
        return;
    }
    norm = sqrtf(norm);
    //
    float cos_h = cosf(norm/2);
    m_value[0] = cos_h;
    norm = sqrtf(1-cos_h*cos_h) / norm; // 用sqrt比sin快。
    m_value[1] = vector[0] * norm;
    m_value[2] = vector[1] * norm;
    m_value[3] = vector[2] * norm;
}

// 相乘。
void Quaternion::multiply(Quaternion const & right)
{
    float tmp[4];
    tmp[0] = m_value[0] * right.m_value[0] - m_value[1] * right.m_value[1] - m_value[2] * right.m_value[2] - m_value[3] * right.m_value[3];
    tmp[1] = m_value[1] * right.m_value[0] + m_value[0] * right.m_value[1] + m_value[2] * right.m_value[3] - m_value[3] * right.m_value[2];
    tmp[2] = m_value[2] * right.m_value[0] + m_value[0] * right.m_value[2] + m_value[3] * right.m_value[1] - m_value[1] * right.m_value[3];
    tmp[3] = m_value[3] * right.m_value[0] + m_value[0] * right.m_value[3] + m_value[1] * right.m_value[2] - m_value[2] * right.m_value[1];
    //
    for(int i=0;i<4;i++)
        m_value[i] = tmp[i];
}

// 取逆。
void Quaternion::inversion()
{
    for(int i=1;i<4;i++)
        m_value[i] = - m_value[i];
}

void Quaternion::assign(float w, float x, float y, float z)
{
    m_value[0] = w;
    m_value[1] = x;
    m_value[2] = y;
    m_value[3] = z;
}

void Quaternion::assign(const float val[4])
{
    for(int i=0;i<4;i++)
        m_value[i] = val[i];
}

// http://www.cppblog.com/heath/archive/2009/12/13/103127.html
void Quaternion::fromAngle(float yaw, float pitch, float roll)
{
    float sin_yaw2 = sin(yaw/2);
    float cos_yaw2 = cos(yaw/2);
    float sin_pitch2 = sin(pitch/2);
    float cos_pitch2 = cos(pitch/2);
    float sin_roll2 = sin(roll/2);
    float cos_roll2 = cos(roll/2);
    //
    m_value[0] = cos_roll2*cos_pitch2*cos_yaw2 + sin_roll2*sin_pitch2*sin_yaw2;
    m_value[1] = sin_roll2*cos_pitch2*cos_yaw2 - cos_roll2*sin_pitch2*sin_yaw2;
    m_value[2] = cos_roll2*sin_pitch2*cos_yaw2 + sin_roll2*cos_pitch2*sin_yaw2;
    m_value[3] = cos_roll2*cos_pitch2*sin_yaw2 - sin_roll2*sin_pitch2*cos_yaw2;
}
