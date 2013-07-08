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
