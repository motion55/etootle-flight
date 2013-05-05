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

#include "glview.h"
#include <math.h>
#include <GL/gl.h>

GlView::GlView()
{
    m_longititude = 0;
    m_latitude = 0;
    m_horizon = 1;
    m_viewPortWidth = 0;
    m_viewPortHeight = 0;
    for(int i=0;i<3;i++)
        m_viewPoint[i] = 0;
}

void GlView::useViewMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(m_latitude, 1.0, 0.0, 0.0);
    glRotatef(m_longititude, 0.0, 0.0, 1.0);
    glTranslatef(-m_viewPoint[0],-m_viewPoint[1],-m_viewPoint[2]);
}

// 视角经度增加delta度。
void GlView::changeLongititude(float delta)
{
    m_longititude += delta;
    //
    if(m_longititude < 0)
        m_longititude += 360;
    if(m_longititude > 360)
        m_longititude -= 360;
}

// 视角纬度增加delta度。
void GlView::changeLatitude(float delta)
{
    m_latitude += delta;
    //
    if(m_latitude < 0)
        m_latitude += 360;
    if(m_latitude > 360)
        m_latitude -= 360;
}

void GlView::setLongititude(float longititude)
{
    m_longititude = longititude;
    //
    if(m_longititude < 0)
        m_longititude += 360;
    if(m_longititude > 360)
        m_longititude -= 360;
}

void GlView::setLatitude(float latitude)
{
    m_latitude = latitude;
    //
    if(m_latitude < 0)
        m_latitude += 360;
    if(m_latitude > 360)
        m_latitude -= 360;
}

void GlView::zoom(float times)
{
    m_horizon *= times;
    //
    updateViewPortAndProjection();
}

float GlView::getHorizon() const
{
    return m_horizon;
}

void GlView::getViewPoint(float center[3]) const
{
    for(int i=0;i<3;i++)
        center[i] = m_viewPoint[i];
}

void GlView::resize(int width, int height)
{
    if(width < 0 || height < 0)
        return;
    //
    m_viewPortWidth = width;
    m_viewPortHeight = height;
    //
    updateViewPortAndProjection();
}

void GlView::updateViewPortAndProjection()
{
    glViewport(0,0,m_viewPortWidth,m_viewPortHeight);
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(m_viewPortWidth < m_viewPortHeight)
    {
        float tmp = m_horizon * m_viewPortHeight / m_viewPortWidth;
        glOrtho(-m_horizon, m_horizon, -tmp, tmp, -16.0 * m_horizon, 16.0 * m_horizon);
        //glFrustum(-m_horizon, +m_horizon, -tmp, tmp, 10 * m_horizon, 14 * m_horizon);
        glTranslatef(0,0,-12 * m_horizon);
    }
    else
    {
        float tmp = m_horizon * m_viewPortWidth / m_viewPortHeight;
        glOrtho(-tmp, tmp, -m_horizon, +m_horizon, -16.0 * m_horizon, 16.0 * m_horizon);
        //glFrustum(-tmp, tmp, -m_horizon, +m_horizon, 10 * m_horizon, 14 * m_horizon);
        glTranslatef(0,0,-12 * m_horizon);
    }
    glMatrixMode(GL_MODELVIEW);
}

void GlView::setHorizon(float sizeHalf)
{
    m_horizon = sizeHalf;
    updateViewPortAndProjection();
}

void GlView::setViewPoint(const float p[])
{
    for(int i=0;i<3;i++)
        m_viewPoint[i] = p[i];
}

void GlView::setViewPoint(float x, float y, float z)
{
    m_viewPoint[0] = x;
    m_viewPoint[1] = y;
    m_viewPoint[2] = z;
}
