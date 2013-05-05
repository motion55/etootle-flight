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

#include "glwidget.h"
#include "globject/material.h"
#include "globject/gltext.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent),
    quadcopter(1),
    m_modelAxes(0.5),
    m_translationTrace(100),
    m_autoScope(m_glView)
{
    GlText::setGlwidget(this);
    //
    m_glView.changeLatitude(-45);
    m_glView.changeLongititude(-135);
    m_needUpdate = false;
    m_translationTrace.setLineWidth(2);
    m_needDisplayWall = true;
    m_needDisplayModel = true;
    m_needDisplayModelAxes = false;
    m_needDisplayWorldAxes = false;
    m_angleDegree_yaw = 0;
    m_angleDegree_pitch = 0;
    m_angleDegree_roll = 0;
    //
    for(int i=0;i<3;i++)
        m_position[i] = 0;
    //
    connect(&m_autoScope,SIGNAL(setView(float,float,float,float)),
            this,SLOT(onSetView(float,float,float,float)));
}

void GLWidget::resetView()
{
    onSetView(0,0,0,1);
    m_glView.setLatitude(-45);
    m_glView.setLongititude(-135);
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    glClearColor(0.5,0.5,0.5,1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); // 隐藏面的背面。
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    //
    // 光滑。
    //glEnable(GL_POINT_SMOOTH);
    //glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
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

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // 加载视角矩阵。
    m_glView.useViewMatrix();
    //
    // 画世界坐标系的坐标轴。
    if(m_needDisplayWorldAxes)
        m_worldAxes.draw();
    if(m_needDisplayWall)
        m_axesWall.draw();
    //
    // 画轨迹。
    m_translationTrace.draw();
    //
    // 画四轴飞行器。
    // 先平移，然后乘上姿态矩阵。
    if(m_needDisplayModel || m_needDisplayModelAxes)
    {
        glPushMatrix();
        glTranslatef(m_position[0],m_position[1],m_position[2]);
        Quaternion tmp;
        tmp = m_attitude;
        tmp.inversion();
        float mat[16];
        tmp.toMatrix(mat);
        glMultMatrixf(mat);
        //
        if(m_needDisplayModel)
            quadcopter.draw();
        if(m_needDisplayModelAxes)
            m_modelAxes.draw();
        //
        glPopMatrix();
    }
    //
    // 画文字。
    QString info;
    int y = 10;
    int y_d = 14;
    int x = 10;
    glColor3f(1,1,1);
    renderText(x,y+=y_d,QString::fromUtf8(" 姿态四元数："));
    renderText(x,y+=y_d,info.sprintf("w = %.4f",m_attitude.w()));
    renderText(x,y+=y_d,info.sprintf("x = %.4f",m_attitude.x()));
    renderText(x,y+=y_d,info.sprintf("y = %.4f",m_attitude.y()));
    renderText(x,y+=y_d,info.sprintf("z = %.4f",m_attitude.z()));
    y+=y_d;
    renderText(x,y+=y_d,QString::fromUtf8(" 姿态角(度)："));
    renderText(x,y+=y_d,info.sprintf("yaw = %.4f",m_angleDegree_yaw));
    renderText(x,y+=y_d,info.sprintf("pitch = %.4f",m_angleDegree_pitch));
    renderText(x,y+=y_d,info.sprintf("roll = %.4f",m_angleDegree_roll));
    y+=y_d;
    renderText(x,y+=y_d,QString::fromUtf8(" 位置(米)："));
    renderText(x,y+=y_d,info.sprintf("x = %.2f",m_position[0]));
    renderText(x,y+=y_d,info.sprintf("y = %.2f",m_position[1]));
    renderText(x,y+=y_d,info.sprintf("z = %.2f",m_position[2]));
}

void GLWidget::resizeGL(int width, int height)
{
    m_glView.resize(width,height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    //
    if (event->buttons() & Qt::LeftButton)
    {
        m_glView.changeLatitude(dy);
        m_glView.changeLongititude(dx);
        //
        m_needUpdate = true;
    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() < 0)
        m_glView.zoom(0.8);
    else
        m_glView.zoom(1.25);
    m_axesWall.setSize(m_glView.getHorizon());
    //
    m_needUpdate = true;
}

void GLWidget::displayAttitude(const Quaternion &atti)
{
    m_attitude = atti;
    //
    m_angleDegree_yaw = (180.0/M_PI) * atan2f(atti.w()*atti.z() - atti.x()*atti.y(),
        1.0/2 - atti.x()*atti.x() - atti.z()*atti.z());
    m_angleDegree_pitch = (180.0/M_PI) * asin(2*atti.y()*atti.z() + 2*atti.w()*atti.x());
    m_angleDegree_roll = (180.0/M_PI) * atan2f(atti.w()*atti.y() - atti.x()*atti.z(),
        1.0/2 - atti.x()*atti.x() - atti.y()*atti.y());
    //
    m_needUpdate = true;
}

void GLWidget::displayPosition(float x,float y,float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
    //
    m_translationTrace.push(m_position);
    m_autoScope.updatePosition(m_position);
}

void GLWidget::every30ms()
{
    if(m_needUpdate)
        updateGL();
    m_needUpdate = false;
}

void GLWidget::onSetView(float x, float y, float z,float sizeHalf)
{
    m_glView.setViewPoint(x,y,z);
    m_glView.setHorizon(sizeHalf);
    m_axesWall.setCenter(x,y,z);
    m_axesWall.setSize(sizeHalf);
    m_worldAxes.setSize(sizeHalf);
    //
    m_needUpdate = true;
}

void GLWidget::needDisplayWall(bool yes)
{
    m_needDisplayWall = yes;
    m_needUpdate = true;
}

void GLWidget::needDisplayModel(bool yes)
{
    m_needDisplayModel = yes;
    m_needUpdate = true;
}

void GLWidget::needDisplayModelAxes(bool yes)
{
    m_needDisplayModelAxes = yes;
    m_needUpdate = true;
}

void GLWidget::needDisplayWorldAxes(bool yes)
{
    m_needDisplayWorldAxes = yes;
    m_needUpdate = true;
}
