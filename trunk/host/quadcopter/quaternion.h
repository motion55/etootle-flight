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

#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion
{
private:
    const static float ZERO = 1e-4;
public:
    Quaternion();
    Quaternion(float w,float x,float y,float z);
    Quaternion(float yaw,float pitch,float roll);
    Quaternion(const float val[4]);
    void toMatrix(float mat[16]);
    void nomalize(void);
    void loadIdentity();
    void fromVectorRotation(const float vector[3]);
    void fromAngle(float yaw,float pitch,float roll);
    void multiply(const Quaternion &right);
    void inversion();
    void assign(float w,float x,float y,float z);
    void assign(const float val[]);
    float & w() {return m_value[0];}
    float & x() {return m_value[1];}
    float & y() {return m_value[2];}
    float & z() {return m_value[3];}
    const float & w() const {return m_value[0];}
    const float & x() const {return m_value[1];}
    const float & y() const {return m_value[2];}
    const float & z() const {return m_value[3];}
    const float * value() const {return m_value;}
private:
    float m_value[4];
};

#endif // QUATERNION_H
