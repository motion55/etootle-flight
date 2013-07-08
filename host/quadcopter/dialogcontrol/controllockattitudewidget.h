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

#ifndef CONTROLLOCKATTITUDEWIDGET_H
#define CONTROLLOCKATTITUDEWIDGET_H

#include <QtGui>
#include <QTimer>
#include "quaternion.h"

class ControlLockAttitudeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlLockAttitudeWidget(QWidget *parent = 0);
    void setHorizontalSensitivity(float sen);
    void reset(void);
    const Quaternion & getControl(void) {return m_control;}

signals:
    void controlChanged(void);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    void refreshControl();

private:
    float m_posX;
    float m_posY;
    QPoint m_lastPos;
    Quaternion m_control;
    float m_horizontalSensitivity; // 弧度/像素。
};

#endif // CONTROLLOCKATTITUDEWIDGET_H
