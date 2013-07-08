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

#ifndef WIDGETVIEW_H
#define WIDGETVIEW_H

#include <QGLWidget>
#include <QTimer>
#include "../globject/glarrow.h"
#include "../globject/glpoints.h"
#include "../globject/glaxes.h"
#include "../globject/glgrid.h"

class WidgetView : public QGLWidget
{
    Q_OBJECT
public:
    explicit WidgetView(QWidget *parent = 0);
    ~WidgetView();

public slots:
    void every30ms();
    void onDirectionChanged(float x,float y,float z);
    void addPoint(float x,float y,float z);
    void clearPoints();
    void needDrawEllipsoid(bool yes);
    void setEllipsoid(float ox,float oy,float oz,float rx,float ry,float rz);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint lastPos;
    float viewAngle_x;
    float viewAngle_z;
    GlArrow arrow;
    GlAxes m_axes;
    GlPoints m_points;
    bool m_needUpdate;
    QTimer m_timer;
    float m_zoom;
    float m_zoomTmp;
    float m_viewPortWidth,m_viewPortHeight;
    GlGrid m_ellipsoid;
    float m_ellipsoidOffset[3];

private:
    void refleshZoom();
    void adjustZoom(float x, float y, float z);
    void adjustZoomTem(float x, float y, float z);
};

#endif // WIDGETVIEW_H
