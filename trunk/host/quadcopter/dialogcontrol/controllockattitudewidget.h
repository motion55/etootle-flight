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
