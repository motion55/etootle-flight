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
