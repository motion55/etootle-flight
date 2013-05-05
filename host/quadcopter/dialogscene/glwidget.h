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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include "quaternion.h"
#include "globject/glquadcopter.h"
#include "globject/gltrace.h"
#include "glview.h"
#include "globject/glaxeswall.h"
#include "autoscope.h"
#include "globject/glaxes.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    void resetView();
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QPoint lastPos;
    Quaternion m_attitude;
    float m_position[3];
    float m_angleDegree_yaw;
    float m_angleDegree_pitch;
    float m_angleDegree_roll;
    GlQuadcopter quadcopter;
    GlAxes m_modelAxes;
    GlAxes m_worldAxes;
    GlTrace m_translationTrace;
    QTimer m_timer;
    bool m_needUpdate;
    GlView m_glView;
    GlAxesWall m_axesWall;
    QFont m_infoFont;
    bool m_needDisplayWall;
    bool m_needDisplayModel;
    bool m_needDisplayModelAxes;
    bool m_needDisplayWorldAxes;

signals:
    
public slots:
    void displayAttitude(const Quaternion &atti);
    void displayPosition(float x,float y,float z);
    void onSetView(float x,float y,float z,float sizeHalf);
    void needDisplayWall(bool yes);
    void needDisplayModel(bool yes);
    void needDisplayModelAxes(bool yes);
    void needDisplayWorldAxes(bool yes);

private slots:
    void every30ms();

private :
    void updateView(const float newPosition[3]);
    AutoScope m_autoScope;
};

#endif // GLWIDGET_H
