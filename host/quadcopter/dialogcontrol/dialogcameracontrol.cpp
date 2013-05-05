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

#include "dialogcameracontrol.h"
#include "ui_dialogcameracontrol.h"
#include "protocol.h"
#include <math.h>
#include "mainwindow.h"

#define radianToDegree(r) ((r) * 180.0f/3.1415926f)
#define degreeToRadian(r) ((r) * 3.1415926f/180.0f)

static const float RADIAN_PER_PIXEL_DEFAULT = degreeToRadian(0.1f/* 0.1度/像素 */);
static const float P_DEFAULT = 1;
static const float I_DEFAULT = 0;
static const float D_DEFAULT = 0;
static const float CLIPPING_PIXEL_DEFAULT = 10.0f/* 限幅10度 */ / \
        radianToDegree(RADIAN_PER_PIXEL_DEFAULT);

float DialogCameraControl::Pid::p = P_DEFAULT;
float DialogCameraControl::Pid::i = I_DEFAULT;
float DialogCameraControl::Pid::d = D_DEFAULT;
float DialogCameraControl::Pid::m_clipping_pixel = CLIPPING_PIXEL_DEFAULT;

DialogCameraControl::DialogCameraControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCameraControl),
    m_editingParam(false),
    m_radianPerPixel(RADIAN_PER_PIXEL_DEFAULT)
{
    ui->setupUi(this);
    getParamFromUI();
    //
    m_currentPosition[0] = m_currentPosition[1] = 0;
    m_targetPosition[0] = m_targetPosition[1] = 0;
    //
    connect(&m_connection,SIGNAL(connected()),
            this,SLOT(onConnected()));
    connect(&m_connection,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(onError(QAbstractSocket::SocketError)));
    connect(&m_connection,SIGNAL(readyRead()),
            this,SLOT(onReadyRead()));
}

DialogCameraControl::~DialogCameraControl()
{
    delete ui;
}

void DialogCameraControl::on_btConnect_clicked()
{
    if(! m_connection.isOpen())
    {
        int port = getIntFromLineEdit(ui->lePort,4892);
        if(port < 1024 || port > 0xFFFF)
        {
            QMessageBox::critical(
                        this,
                        QString::fromUtf8("连接失败"),
                        QString::fromUtf8("端口不合理或已被占用。"));
            return;
        }
        m_connection.connectToHost(ui->leServer->text(),port);
        //
        ui->leServer->setEnabled(false);
        ui->lePort->setEnabled(false);
        ui->btConnect->setEnabled(false);
        ui->btConnect->setText(QString::fromUtf8("尝试连接"));
    }
    else
    {
        m_connection.close();
        ui->leServer->setEnabled(true);
        ui->lePort->setEnabled(true);
        ui->btConnect->setText(QString::fromUtf8("连接"));
    }
}

void DialogCameraControl::onConnected()
{
    m_connection.setSocketOption(QAbstractSocket::LowDelayOption,QVariant(1));
    //
    ui->btConnect->setText(QString::fromUtf8("断开"));
    ui->btConnect->setEnabled(true);
}

void DialogCameraControl::onError(QAbstractSocket::SocketError error)
{
    ui->leServer->setEnabled(true);
    ui->lePort->setEnabled(true);
    ui->btConnect->setText(QString::fromUtf8("连接"));
    ui->btConnect->setEnabled(true);
    //
    MainWindow::printMessage(QString::fromUtf8("摄像头定点 : ") + m_connection.errorString());
    m_connection.close();
}

void DialogCameraControl::onReadyRead()
{
    if(m_binaryParser.parse(m_connection.readAll()))
    {
        // 收到新坐标。
        onNewPostion(m_binaryParser.x(),m_binaryParser.y());
    }
}

int DialogCameraControl::getIntFromLineEdit(QLineEdit *le, int _default)
{
    bool ok = false;
    int val = le->text().toInt(&ok);
    //
    if(!ok)
    {
        val = _default;
        le->setText(QString("%1").arg(val));
    }
    //
    return val;
}

float DialogCameraControl::getFloatFromLineEdit(QLineEdit *le, float _default)
{
    bool ok = false;
    float val = le->text().toFloat(&ok);
    //
    if(!ok)
    {
        val = _default;
        le->setText(QString("%1").arg(val));
    }
    //
    return val;
}

void DialogCameraControl::onNewPostion(float x, float y)
{
    m_currentPosition[0] = m_currentPosition[0] * 0.2 + x * 0.8;
    m_currentPosition[1] = m_currentPosition[1] * 0.2 + y * 0.8;
    //
    float dx = m_targetPosition[0] - m_currentPosition[0];
    float dy = m_targetPosition[1] - m_currentPosition[1];
    float dr = (float)sqrt(dx*dx+dy*dy);
    float angle = radianToDegree(atan2(dy,dx));
    //
    m_pid[0].iterate(dx);
    m_pid[1].iterate(dy);
    refleshControl();
    //
    ui->leCurrentPosition->setText(QString("%1, %2")
                .arg(m_currentPosition[0],-10)
                .arg(m_currentPosition[1],-10));
    ui->leDeviationXy->setText(QString("%1, %2").arg(dx,-10).arg(dy,-10));
    ui->leDeviationPole->setText(QString("%1< %2").arg(dr,-10).arg(angle,-10));
}

int DialogCameraControl::getParamFromUI()
{
    Pid::p = getFloatFromLineEdit(ui->leP,P_DEFAULT);
    Pid::i = getFloatFromLineEdit(ui->leI,I_DEFAULT);
    Pid::d = getFloatFromLineEdit(ui->leD,D_DEFAULT);
    //
    m_radianPerPixel = degreeToRadian(getFloatFromLineEdit
            (ui->leScale,radianToDegree(RADIAN_PER_PIXEL_DEFAULT)));
    Pid::m_clipping_pixel = degreeToRadian(getFloatFromLineEdit
            (ui->leClipping,radianToDegree(CLIPPING_PIXEL_DEFAULT * m_radianPerPixel)))
                                            / m_radianPerPixel;
    //
    refleshControl();
    //
    return 0;
}

void DialogCameraControl::refleshControl()
{
    m_control.assign(
                1,
                -m_pid[1].m_current * m_radianPerPixel,
                +m_pid[0].m_current * m_radianPerPixel,
                0);
    m_control.nomalize();
    emit onControlChanged();
}

DialogCameraControl::BinaryParser::BinaryParser()
{
    m_step = FINDING_PROTOCOL_VERSION;
}

int DialogCameraControl::BinaryParser::parse(QByteArray input)
{
    int newXY = 0;
    //
    for(int i=0;i<input.size();i++)
    {
        uint8_t byte = input.at(i);
        //
        switch(m_step)
        {
        case FINDING_PROTOCOL_VERSION:
            if(byte == Protocol::VERSION)
                m_step = FINDING_FRAME_TYPE;
            break;
        case FINDING_FRAME_TYPE:
            if(byte == Protocol::POSITION_IN_CAMERA)
                m_step = FINDING_UNIT;
            else if(byte != Protocol::VERSION)
                m_step = FINDING_PROTOCOL_VERSION;
            break;
        case FINDING_UNIT:
            if(byte == Protocol::PositionInCamera::PIXEL_UNIT)
            {
                m_step = GETTING_DATA;
                m_bytesValid = 0;
            }
            else
                m_step = FINDING_PROTOCOL_VERSION;
            break;
        case GETTING_DATA:
            m_bytes[m_bytesValid ++] = byte;
            if(m_bytesValid >= 8)
            {
                newXY = 1;
                m_step = FINDING_PROTOCOL_VERSION;
            }
            break;
        default:
            m_step = FINDING_PROTOCOL_VERSION;
            break;
        }
    }
    //
    return newXY;
}

void DialogCameraControl::on_btTargetAssignCurrent_clicked()
{
    m_targetPosition[0] = m_currentPosition[0];
    m_targetPosition[1] = m_currentPosition[1];
    //
    ui->leTargetPosition->setText(QString("%1, %2")
                .arg(m_targetPosition[0],-10)
                .arg(m_targetPosition[1],-10));
}

void DialogCameraControl::on_btLockUnlockParam_clicked()
{
    m_editingParam = ! m_editingParam;
    if(m_editingParam)
    {
        ui->btLockUnlockParam->setText(QString::fromUtf8("应用参数"));
    }
    else
    {
        // 从界面读参数。
        getParamFromUI();
        ui->btLockUnlockParam->setText(QString::fromUtf8("解锁参数"));
    }
    //
    ui->leP->setEnabled(m_editingParam);
    ui->leI->setEnabled(m_editingParam);
    ui->leD->setEnabled(m_editingParam);
    ui->leScale->setEnabled(m_editingParam);
    ui->leClipping->setEnabled(m_editingParam);
}

float DialogCameraControl::Pid::clip(float v)
{
    if(v > m_clipping_pixel)
        v = m_clipping_pixel;
    if(v < -m_clipping_pixel)
        v = -m_clipping_pixel;
    return v;
}

DialogCameraControl::Pid::Pid()
{
    m_current = 0;
    m_pre = 0;
    m_sum = 0;
}

float DialogCameraControl::Pid::iterate(float input)
{
    m_sum = clip(m_sum + input);
    //
    float p_v = clip(input * p);
    float i_v = clip(m_sum * i);
    float d_v = clip((input - m_pre) * d);
    //
    m_current = clip(p_v + i_v + d_v);
    //
    return m_current;
}
