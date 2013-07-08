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

#include <Qt>
#include "dialogcontrol.h"
#include "ui_dialogcontrol.h"
#include "protocol.h"
#include <QSettings>

static const char * SETTINGS_MOUSE_SENSITITIVITY = "control/mouse_sensitivity";

DialogControl::DialogControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogControl),
    m_currentMode(HALT),
    m_baseThrottle(0),
    m_dialogCameraControl(this),
    m_lockAttitudeCanSend(true),
    m_lockAttitudeNeedSend(false)
{
    ui->setupUi(this);
    loadSettings();
    //
    m_lockAttitudeTimer.setInterval(30);
    m_lockAttitudeTimer.start(100);
    //
    connect(&m_heartbeatTimer,SIGNAL(timeout()),this,SLOT(onHeartbeatTimer()));
    connect(ui->lockAttitude_widget,SIGNAL(controlChanged()),
            this,SLOT(lockAttitude_onControlChanged()));
    connect(&m_dialogCameraControl,SIGNAL(onControlChanged()),
            this,SLOT(lockAttitude_onControlChanged()));
    connect(&m_lockAttitudeTimer,SIGNAL(timeout()),
            this,SLOT(onLockAttitudeTimer()));
    //
}

DialogControl::~DialogControl()
{
    delete ui;
}

void DialogControl::keyPressEvent(QKeyEvent * event)
{
    if(m_currentMode != LOCK_ATTITUDE)
    {
        QDialog::keyPressEvent(event);
        return;
    }
    //
    switch(event->key())
    {
    case Qt::Key_Up :
    case Qt::Key_W :
        lockAttitude_changeThrottle(1);
        break;
    case Qt::Key_Down :
    case Qt::Key_S :
        lockAttitude_changeThrottle(-1);
        break;
    case Qt::Key_PageUp :
        lockAttitude_changeThrottle(5);
        break;
    case Qt::Key_PageDown :
        lockAttitude_changeThrottle(-5);
        break;
    case Qt::Key_End :
    case Qt::Key_Escape :
        ui->lockAttitude_throttleBase->setValue
                (ui->lockAttitude_throttleBase->minimum());
        lockAttitude_sendControl();
        break;
    }
}

void DialogControl::wheelEvent(QWheelEvent *event)
{
}

void DialogControl::lockAttitude_changeThrottle(int delta)
{
    ui->lockAttitude_throttleBase->setValue
            (ui->lockAttitude_throttleBase->value() + delta);
}

void DialogControl::on_lockAttitude_throttleBase_valueChanged(int value)
{
    m_baseThrottle = (float)value / ui->lockAttitude_throttleBase->maximum();
    ui->lockAttitude_throttleValue->setText(QString().sprintf("%.3f",m_baseThrottle));
    //
    lockAttitude_onControlChanged();
}

void DialogControl::on_setMode_halt_clicked()
{
    ui->lockThrottle->setEnabled(false);
    ui->lockAttitude->setEnabled(false);
    //
    emit setMode(Protocol::ControlMode::HALT);
    m_currentMode = HALT;
}

void DialogControl::on_setMode_lockThrottle_clicked()
{
    ui->lockThrottle->setEnabled(true);
    ui->lockAttitude->setEnabled(false);
    //
    emit setMode(Protocol::ControlMode::LOCK_THROTTLE);
    m_currentMode = LOCK_THROTTLE;
}

void DialogControl::on_setMode_lockAttitude_clicked()
{
    ui->lockThrottle->setEnabled(false);
    ui->lockAttitude->setEnabled(true);
    //
    emit setMode(Protocol::ControlMode::LOCK_ATTITUDE);
    m_currentMode = LOCK_ATTITUDE;
}

void DialogControl::on_lockThrottle_slider0_valueChanged(int value)
{
    lockThrottle_changed(value);
}

void DialogControl::on_lockThrottle_slider1_valueChanged(int value)
{
    lockThrottle_changed(value);
}

void DialogControl::on_lockThrottle_slider2_valueChanged(int value)
{
    lockThrottle_changed(value);
}

void DialogControl::on_lockThrottle_slider3_valueChanged(int value)
{
    lockThrottle_changed(value);
}

void DialogControl::lockThrottle_changed(int val)
{
    static QSlider * slider[4] =
    {
        ui->lockThrottle_slider0,
        ui->lockThrottle_slider1,
        ui->lockThrottle_slider2,
        ui->lockThrottle_slider3,
    };
    static QLabel * text[4] =
    {
        ui->lockThrottle_text0,
        ui->lockThrottle_text1,
        ui->lockThrottle_text2,
        ui->lockThrottle_text3,
    };
    //
    QVector<float> th(4);
    //
    if(ui->lockThrottle_same->isChecked())
    {
        for(int i=0;i<4;i++)
            slider[i]->setValue(val);
    }
    for(int i=0;i<4;i++)
    {
        th[i] = (float)slider[i]->value() / slider[i]->maximum();
        text[i]->setText(QString().sprintf("%.3f",th[i]));
    }
    emit lockThrottle_setThrottle(th);
}

unsigned int DialogControl::getUint(QLineEdit *from, unsigned int default_)
{
    bool ok = true;
    int v = from->text().toInt(&ok);
    //
    // 成功就返回。
    if(ok && v >= 0)
        return v;
    //
    // 失败就重设文本。
    QString str;
    from->setText(str.sprintf("%d",default_));
    return default_;
}

float DialogControl::getFloat(QLineEdit *from, float default_)
{
    bool ok = true;
    float v = from->text().toFloat(&ok);
    //
    // 成功就返回。
    if(ok && v >= 0)
        return v;
    //
    // 失败就重设文本。
    QString str;
    from->setText(str.sprintf("%g",default_));
    return default_;
}

void DialogControl::lockAttitude_sendControl()
{
    QByteArray send;
    send.append(Protocol::LockAttitude::CONTROL);
    send.append((const char *)&m_baseThrottle,4);
    send.append((const char *)lockAttitude_getControlAttitude().value(),4*4);
    //
    emit lockAttitude(send);
}

void DialogControl::loadSettings()
{
    QSettings settings;
    //
    ui->lockAttitude_hsSensitivity->setValue
            (settings.value(SETTINGS_MOUSE_SENSITITIVITY,QVariant(49)).toInt());
}

Quaternion DialogControl::lockAttitude_getControlAttitude()
{
    Quaternion all;
    if(ui->lockAttitude_cbCameraControl->isChecked())
    {
        all = m_dialogCameraControl.getControl();
        all.multiply(ui->lockAttitude_widget->getControl());
    }
    else
        all = ui->lockAttitude_widget->getControl();
    //
    return all;
}

void DialogControl::on_heartbeat_enable_clicked(bool checked)
{
    if(checked)
    {
        int interval = getUint(ui->heartbeat_interval,500);
        m_heartbeatTimer.setInterval(interval);
        m_heartbeatTimer.start();
        //
        // 界面。
        ui->heartbeat_interval->setEnabled(false);
    }
    else
    {
        m_heartbeatTimer.stop();
        //
        // 界面。
        ui->heartbeat_interval->setEnabled(true);
    }
}

void DialogControl::onHeartbeatTimer()
{
    emit sendHeartbeat();
}

void DialogControl::lockAttitude_onControlChanged()
{
    if(m_currentMode != LOCK_ATTITUDE)
        return;
    //
    if(m_lockAttitudeCanSend)
    {
        lockAttitude_sendControl();
        m_lockAttitudeNeedSend = false;
        m_lockAttitudeCanSend = false;
    }
    else
    {
        m_lockAttitudeNeedSend = true;
    }
}

void DialogControl::on_lockAttitude_hsSensitivity_valueChanged(int value)
{
    float sensitivity = (value + 1) * 0.0001;
    ui->lockAttitude_lSensitivity->setText(QString("%1").arg(sensitivity));
    //
    QSettings settings;
    settings.setValue(SETTINGS_MOUSE_SENSITITIVITY,QVariant(value));
    //
    ui->lockAttitude_widget->setHorizontalSensitivity(sensitivity);
}

void DialogControl::on_lockAttitude_btReset_clicked()
{
    ui->lockAttitude_throttleBase->setValue(0);
    on_lockAttitude_throttleBase_valueChanged(0);
    ui->lockAttitude_widget->reset();
    //
    lockAttitude_onControlChanged();
}

void DialogControl::onLockAttitude(const QByteArray &param)
{
}

void DialogControl::on_btGetReady_clicked()
{
    QByteArray send;
    send.append(Protocol::LockAttitude::GET_READY);
    emit lockAttitude(send);
}

void DialogControl::on_lockAttitude_cbCameraControl_stateChanged(int arg1)
{
    ui->lockAttitude_btCameraSetting->setEnabled(arg1);
}

void DialogControl::on_lockAttitude_btCameraSetting_clicked()
{
    m_dialogCameraControl.showNormal();
    m_dialogCameraControl.raise();
}

void DialogControl::onLockAttitudeTimer()
{
    if(m_lockAttitudeNeedSend)
    {
        lockAttitude_sendControl();
    }
    m_lockAttitudeCanSend = true;
    m_lockAttitudeNeedSend = false;
}

void DialogControl::on_btMakeHorizontalAttitude_clicked()
{
    emit makeHorizontalAttitude(lockAttitude_getControlAttitude());
}
