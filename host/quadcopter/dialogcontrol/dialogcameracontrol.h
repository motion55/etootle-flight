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

#ifndef DIALOGCAMERACONTROL_H
#define DIALOGCAMERACONTROL_H

#include <QtGui>
#include <QtNetwork>
#include "quaternion.h"

namespace Ui {
class DialogCameraControl;
}

class DialogCameraControl : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCameraControl(QWidget *parent = 0);
    ~DialogCameraControl();
    const Quaternion & getControl(void) {return m_control;}

signals:
    void onControlChanged(void);

private slots:
    void on_btConnect_clicked();
    void onConnected(void);
    void onError(QAbstractSocket::SocketError error);
    void onReadyRead(void);
    void on_btTargetAssignCurrent_clicked();
    void on_btLockUnlockParam_clicked();

private:
    int getIntFromLineEdit(QLineEdit * le,int _default=0);
    float getFloatFromLineEdit(QLineEdit * le,float _default=0);
    void onNewPostion(float x,float y);
    int getParamFromUI(void);
    void refleshControl(void);

private:
    Ui::DialogCameraControl *ui;
    QTcpSocket m_connection;
    float m_currentPosition[2];
    float m_targetPosition[2];
    bool m_editingParam;
    float m_radianPerPixel;
    Quaternion m_control;

private:
    class BinaryParser
    {
    public:
        BinaryParser();
        int parse(QByteArray input);
        float x() const {return m_xy[0];}
        float y() const {return m_xy[1];}
    private:
        enum
        {
            FINDING_PROTOCOL_VERSION = 1,
            FINDING_FRAME_TYPE,
            FINDING_UNIT,
            GETTING_DATA
        }m_step;
        int m_bytesValid;
        union
        {
            float m_xy[2];
            uint8_t m_bytes[8];
        };
    }m_binaryParser;

private:
    class Pid
    {
    public:
        Pid();
        float iterate(float input);
    public:
        static float p,i,d;
        static float m_clipping_pixel; // 限幅，单位：像素。
        float m_current;
    private:
        float clip(float v);
    private:
        float m_pre;
        float m_sum;
    }m_pid[2];
};

#endif // DIALOGCAMERACONTROL_H
