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

#include "glaxes.h"
#include "material.h"
#include <GL/gl.h>

GlAxes::GlAxes(float size)
    : m_size(size)
{
}

void GlAxes::setSize(float size)
{
    if(size > 0)
        m_size = size;
}

void GlAxes::draw()
{
    const float LENGTH = m_size;
    const float ARROW = 0.1f * m_size;
    const float HEIGHT = 0.06f * m_size;
    //
    glLineWidth(1);
    //
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    {
        glColor3f(1,0,0);
        glVertex3f(-LENGTH,0.0,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),HEIGHT,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),-HEIGHT,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),0.0,HEIGHT);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),0.0,-HEIGHT);
        //
        glColor3f(0,1,0);
        glVertex3f(0.0,-LENGTH,0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,(LENGTH - ARROW),HEIGHT);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,(LENGTH - ARROW),-HEIGHT);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(HEIGHT,(LENGTH - ARROW),0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(-HEIGHT,(LENGTH - ARROW),0.0);
        //
        glColor3f(0,0,1);
        glVertex3f(0.0,0.0,-LENGTH);
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(HEIGHT,0.0,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(-HEIGHT,0.0,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,HEIGHT,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,-HEIGHT,(LENGTH - ARROW));
    }glEnd();
    glEnable(GL_LIGHTING);
}
