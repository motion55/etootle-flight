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

#include <QtGui>
#include <QtOpenGL>
#include <math.h>

#include "widgetview.h"
#include "globject/glaxes.h"
#include "globject/material.h"
#include "GL/glu.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

WidgetView::WidgetView(QWidget *parent) :
    QGLWidget(parent)
{
    viewAngle_x = -45;
    viewAngle_z = -135;
    m_needUpdate = false;
    m_viewPortWidth = 0;
    m_viewPortHeight = 0;
    m_zoom = 1;
    m_zoomTmp = 1;
    for(int i=0;i<3;i++)
        m_ellipsoidOffset[i] = 0;
    m_ellipsoid.make(24,24);
    m_ellipsoid.LoadSphere(1,1,1);
    m_ellipsoid.getMaterialSurface()->loadPearl();
    m_ellipsoid.getMaterialSurface()->moreTransparency(0.4);
    //
    m_points.setPointSize(2);
}

WidgetView::~WidgetView()
{
}

void WidgetView::initializeGL()
{
    glClearColor(0.5,0.5,0.5,1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); // 隐藏面的背面。
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    //
    // 光滑。
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // 线优先，把线向前偏移。
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonOffset(0.6,1.1);
    //
    // 开启光源。
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //
    // 光源位置。
    static GLfloat lightPosition[4] = {1e6,1e6,1e6,1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    //
    // 光源颜色，白色。
    const GLfloat light0_ambient[4] = {1.0,1.0,1.0,1.0};
    const GLfloat light0_diffuse[4] = {1.0,1.0,1.0,1.0};
    const GLfloat light0_specular[4] = {1.0,1.0,1.0,1.0};
    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
    //
    // 启动计时器。
    m_timer.setInterval(30);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(every30ms()));
    m_timer.start();
}

void WidgetView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // 加载视角矩阵。
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(viewAngle_x, 1.0, 0.0, 0.0);
    glRotatef(viewAngle_z, 0.0, 0.0, 1.0);
    //
    // 画世界坐标系的参照物。
    m_axes.draw();
    //
    arrow.draw();
    m_points.draw();
    //
    glTranslatef(-m_ellipsoidOffset[0],-m_ellipsoidOffset[1],-m_ellipsoidOffset[2]);
    m_ellipsoid.draw();
}

void WidgetView::resizeGL(int width, int height)
{
    m_viewPortWidth = width;
    m_viewPortHeight = height;
    //
    glViewport(0,0,width,height);
    refleshZoom();
    //
    m_needUpdate = true;
}

void WidgetView::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void WidgetView::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    //
    if (event->buttons() & Qt::LeftButton)
    {
        viewAngle_x += dy;
        viewAngle_z += dx;
        //
        m_needUpdate = true;
    }
    lastPos = event->pos();
}

void WidgetView::every30ms()
{
    if(m_needUpdate)
        updateGL();
    m_needUpdate = false;
}

void WidgetView::onDirectionChanged(float x,float y,float z)
{
    arrow.setEndPoint(x,y,z);
    adjustZoomTem(x,y,z);
    //
    m_needUpdate = true;
}

void WidgetView::addPoint(float x, float y, float z)
{
    m_points.push(x,y,z);
    adjustZoom(x,y,z);
}

void WidgetView::adjustZoom(float x,float y,float z)
{
    if(x<0)
        x=-x;
    if(y<0)
        y=-y;
    if(z<0)
        z=-z;
    //
    bool changed = false;
    float new_zoom = m_zoom; // 在原来的基础上增加。
    if(new_zoom < x)
        new_zoom = x,changed = true;
    if(new_zoom < y)
        new_zoom = y,changed = true;
    if(new_zoom < z)
        new_zoom = z,changed = true;
    //
    if(changed)
    {
        m_zoom = new_zoom * 1.2; // 乘以1.2以扩大可视范围。
        refleshZoom();
    }
}

void WidgetView::adjustZoomTem(float x,float y,float z)
{
    if(x<0)
        x=-x;
    if(y<0)
        y=-y;
    if(z<0)
        z=-z;
    //
    bool changed = false;
    float new_zoom = 1; // 每次都重置。
    if(new_zoom < x)
        new_zoom = x,changed = true;
    if(new_zoom < y)
        new_zoom = y,changed = true;
    if(new_zoom < z)
        new_zoom = z,changed = true;
    //
    if(changed)
    {
        m_zoomTmp = new_zoom * 1.2; // 乘以1.2以扩大可视范围。
        refleshZoom();
    }
}

void WidgetView::refleshZoom()
{
    float zoom = 1;
    if(m_zoom >= m_zoomTmp)
        zoom = m_zoom;
    else
        zoom = m_zoomTmp;
    //
    m_axes.setSize(zoom);
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(m_viewPortWidth < m_viewPortHeight)
    {
        float tmp = m_viewPortHeight / m_viewPortWidth;
        glOrtho(-zoom, zoom, -tmp*zoom, tmp*zoom, -16.0*zoom, 16.0*zoom);
    }
    else
    {
        float tmp = m_viewPortWidth / m_viewPortHeight;
        glOrtho(-tmp*zoom, tmp*zoom, -zoom, zoom, -16.0*zoom, 16.0*zoom);
    }
    glMatrixMode(GL_MODELVIEW);
    //
    m_needUpdate = true;
}

void WidgetView::clearPoints()
{
    m_points.clear();
    m_zoom = 1;
    refleshZoom();
}

void WidgetView::needDrawEllipsoid(bool yes)
{
    m_ellipsoid.needDrawSurface(yes);
    //
    m_needUpdate = true;
}

void WidgetView::setEllipsoid(float ox, float oy, float oz, float rx, float ry, float rz)
{
    m_ellipsoidOffset[0] = ox;
    m_ellipsoidOffset[1] = oy;
    m_ellipsoidOffset[2] = oz;
    m_ellipsoid.LoadSphere(rx,ry,rz);
}
