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
