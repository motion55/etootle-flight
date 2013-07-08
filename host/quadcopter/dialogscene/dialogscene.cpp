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

#include "dialogscene.h"
#include "ui_dialogscene.h"

DialogScene::DialogScene(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogScene)
{
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    //
    connect(&m_timerAutoGetAttitude,SIGNAL(timeout()),
            this,SIGNAL(getAttitude()));
    connect(&m_timerAutoGetPosition,SIGNAL(timeout()),
            this,SIGNAL(getPosition()));
}

DialogScene::~DialogScene()
{
    delete ui;
}

void DialogScene::resizeEvent(QResizeEvent * event)
{
    // 默认布局大小不会自动变，要手动设置。
    QSize s = event->size();
    QRect r(0,0,s.width(),s.height());
    ui->horizontalLayoutWidget->setGeometry(r);
}

void DialogScene::closeEvent(QCloseEvent *event)
{
    ui->autoGetAttitude->setCheckState(Qt::Unchecked);
    ui->autoGetPosition->setCheckState(Qt::Unchecked);
}

unsigned int DialogScene::getUintFromLineEdit(QLineEdit *from, unsigned int default_)
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

void DialogScene::displayAttitude(const Quaternion &atti)
{
    if(isVisible())
        emit ui->scene->displayAttitude(atti);
}

void DialogScene::displayPosition(float x, float y, float z)
{
    if(isVisible())
        emit ui->scene->displayPosition(x,y,z);
}

void DialogScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        if(ui->fullscreen->isChecked())
            ui->fullscreen->setChecked(false);
        else
        {
            ui->autoGetAttitude->setCheckState(Qt::Unchecked);
            ui->autoGetPosition->setCheckState(Qt::Unchecked);
            QDialog::keyPressEvent(event);
        }
    }
}

void DialogScene::on_autoGetAttitude_stateChanged(int arg1)
{
    if(arg1)
    {
        // 勾上。
        ui->autoGetAttitude_period->setEnabled(false);
        m_timerAutoGetAttitude.setInterval(getUintFromLineEdit(
            ui->autoGetAttitude_period,50));
        m_timerAutoGetAttitude.start();
    }
    else
    {
        // 取消勾。
        ui->autoGetAttitude_period->setEnabled(true);
        m_timerAutoGetAttitude.stop();
    }
}

void DialogScene::on_autoGetPosition_stateChanged(int arg1)
{
    if(arg1)
    {
        // 勾上。
        ui->autoGetPosition_period->setEnabled(false);
        m_timerAutoGetPosition.setInterval(getUintFromLineEdit(
            ui->autoGetPosition_period,50));
        m_timerAutoGetPosition.start();
    }
    else
    {
        // 取消勾。
        ui->autoGetPosition_period->setEnabled(true);
        m_timerAutoGetPosition.stop();
    }
}

void DialogScene::on_resetView_clicked()
{
    ui->scene->resetView();
}

void DialogScene::on_fullscreen_stateChanged(int arg1)
{
    if(arg1)
        showFullScreen();
    else
        showNormal();
}

void DialogScene::on_displayWall_stateChanged(int arg1)
{
    emit ui->scene->needDisplayWall(arg1);
}

void DialogScene::on_displayModel_stateChanged(int arg1)
{
    emit ui->scene->needDisplayModel(arg1);
}

void DialogScene::on_displayModelAxes_stateChanged(int arg1)
{
    emit ui->scene->needDisplayModelAxes(arg1);
}

void DialogScene::on_displayWorldAxes_stateChanged(int arg1)
{
    emit ui->scene->needDisplayWorldAxes(arg1);
}
