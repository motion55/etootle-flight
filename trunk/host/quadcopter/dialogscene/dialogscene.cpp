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
            QDialog::keyPressEvent(event);;
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
