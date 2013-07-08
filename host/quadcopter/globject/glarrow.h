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

// glvector
// 2012-9-4 19:30:07
// js200300953

#ifndef GLVECTOR_H
#define GLVECTOR_H

#include "globject.h"
#include "material.h"

class GlArrow  : public GlObject
{
public:
    explicit GlArrow(float size = 1);
    virtual void draw(void);
    void setEndPoint(float x,float y,float z);
private:
    float m_vector[3];
    Material m_materialStick,m_materialHead;
    float m_spindle_x;
    float m_spindle_y;
    float m_spindle_angle;
    float m_length;
};

#endif // GLVECTOR_H
