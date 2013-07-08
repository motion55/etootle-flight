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

#include "gltext.h"
#include <string.h>
#include <GL/glu.h>
#include <QGLWidget>

// 定义。
QGLWidget * GlText::m_glWidget = 0;

GlText::GlText()
{
}

void GlText::setText(const QString & text)
{
    m_text = text;
}

void GlText::setFont(const QFont & font)
{
    m_font = font;
}

void GlText::setPosition(const float pos[3])
{
    for(int i=0;i<3;i++)
        m_position[i] = pos[i];
}

void GlText::setPosition(float x,float y,float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void GlText::setGlwidget(QGLWidget *glWidget)
{
    GlText::m_glWidget = glWidget;
}

void GlText::draw()
{
    if(m_text.length() == 0)
        return;
    GlText::m_glWidget->renderText(m_position[0],m_position[1],m_position[2],m_text,m_font);
}
