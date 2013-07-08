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

#include "glarrow.h"
#include <math.h>
#include <GL/glu.h>

GlArrow::GlArrow(float size)
{
    setEndPoint(1,0,0);
    m_materialStick.loadBronze();
    m_materialHead.loadJaspure();
}

void GlArrow::draw()
{
    static GLUquadric * q = gluNewQuadric();
    glPushMatrix();
    {
        glRotatef(m_spindle_angle,m_spindle_x,m_spindle_y,0);
        //
        glEnable(GL_AUTO_NORMAL);
        m_materialStick.useMaterial();
        gluCylinder(q,0,0.03*m_length,0,16,2);
        gluCylinder(q,0.03*m_length,0.03*m_length,0.85*m_length,16,2);
        glTranslatef(0,0,0.85*m_length);
        m_materialHead.useMaterial();
        gluCylinder(q,0.03*m_length,0.06*m_length,0,16,2);
        gluCylinder(q,0.06*m_length,0,0.15*m_length,16,2);
        glDisable(GL_AUTO_NORMAL);
    } glPopMatrix();
}

void GlArrow::setEndPoint(float x,float y,float z)
{
    float x2_y2 = x*x + y*y;
    float sqrt_x2_y2 = sqrtf(x2_y2);
    //
    m_spindle_x = - y / sqrt_x2_y2;
    m_spindle_y = x / sqrt_x2_y2;
    //
    m_spindle_angle = atan2f(sqrt_x2_y2,z) * 180 / M_PI;
    //
    m_length = sqrtf(x2_y2 + z*z);
}
