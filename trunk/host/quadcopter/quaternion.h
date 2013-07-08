//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ==================================================
//         ========圆点博士微型四轴飞行器配套软件声明========
//         ==================================================
//     圆点博士微型四轴飞行器配套软件包括上位机程序、下位机Bootloader
// 、下位机App和遥控程序，及它们的源代码，以下总称“软件”。
//     软件仅提供参考，js200300953不对软件作任何担保，不对因使用该软件
// 而出现的损失负责。
//     软件可以以学习为目的修改和使用，但不允许以商业的目的使用该软件。
//     修改该软件时，必须保留原版权声明。
// 
//     更多资料见：
// http://blog.sina.com.cn/js200300953
// http://www.etootle.com/
// http://www.eeboard.com/bbs/forum-98-1.html#separatorline
// 圆点博士微型四轴飞行器QQ群：276721324

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
