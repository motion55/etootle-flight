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
