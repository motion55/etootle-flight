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

#include "dialogstatus.h"
#include "ui_dialogstatus.h"
#include "protocol.h"

DialogStatus::DialogStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogStatus)
{
    ui->setupUi(this);
    //
    connect(&m_timerGet,SIGNAL(timeout()),this,SLOT(onTimer()));
}

DialogStatus::~DialogStatus()
{
    delete ui;
}

void DialogStatus::on_btGet_clicked()
{
    QVector<uint8_t> v;
    v.append(Protocol::Status::BATERY_VOLTAGE);
    v.append(Protocol::Status::CURRENT_THROTTLE);
    emit getStatus(v);
}

void DialogStatus::onTimer()
{
    on_btGet_clicked();
}

void DialogStatus::onReturnStatus(uint8_t which, const QVector<uint8_t> &value)
{
    switch(which)
    {
    case Protocol::Status::BATERY_VOLTAGE :
        ui->leBateryVoltage->setText(QString("%1").arg(*(float *)value.constData()));
        break;
    case Protocol::Status::CURRENT_THROTTLE :
        {
            float * th = (float *)value.constData();
            ui->teThrottle->clear();
            for(int i=0;i<4;i++)
                ui->teThrottle->append((QString("%1").arg(th[i])));
        }
        break;
    }
}

unsigned int DialogStatus::getUintFromLineEdit(QLineEdit *from, unsigned int default_)
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

void DialogStatus::on_cbAutoGet_stateChanged(int arg1)
{
    if(arg1)
    {
        m_timerGet.setInterval(getUintFromLineEdit(ui->leAutoGetInterval,1000));
        m_timerGet.start();
        //
    }
    else
    {
        m_timerGet.stop();
    }
    //
    ui->leAutoGetInterval->setEnabled(!arg1);
}
