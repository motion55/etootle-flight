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
