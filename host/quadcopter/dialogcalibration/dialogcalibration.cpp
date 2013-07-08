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

#include "dialogcalibration.h"
#include "ui_dialogcalibration.h"
#include <qevent.h>
#include "fitting.h"
#include <QTextStream>

DialogCalibration::DialogCalibration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCalibration)
{
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    setupDataOption();
    //
    m_sampleCountdown = 0;
    m_isSampling = false;
    m_radius = 1;
    m_dataType = Protocol::VectorType::RAW_ACC;
    for(int i=0;i<3;i++)
    {
        m_preOffset[i] = 0;
        m_xyzDisplay[i] = 0;
        m_resultOffset_noPre[i] = 0;
        m_resultScale[i] = 1;
    }
    //
    connect(&m_timerAutoGetData,SIGNAL(timeout()),this,SLOT(onAutoGetDataTimer()));
}

DialogCalibration::~DialogCalibration()
{
    delete ui;
}

void DialogCalibration::resizeEvent(QResizeEvent * event)
{
    // 默认布局大小不会自动变，要手动设置。
    QSize s = event->size();
    QRect r(0,0,s.width(),s.height());
    ui->horizontalLayoutWidget->setGeometry(r);
}

void DialogCalibration::closeEvent(QCloseEvent * event)
{
    ui->autoGetData->setChecked(Qt::Unchecked);
}

void DialogCalibration::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        ui->autoGetData->setChecked(Qt::Unchecked);
    }
    QDialog::keyPressEvent(event);
}

unsigned int DialogCalibration::getUintFromLineEdit(QLineEdit *from, unsigned int default_)
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

float DialogCalibration::getFloatFromLineEdit(QLineEdit * from,float default_)
{
    bool ok = true;
    float v = from->text().toFloat(&ok);
    //
    // 成功就返回。
    if(ok)
        return v;
    //
    // 失败就重设文本。
    QString str;
    from->setText(str.sprintf("%g",default_));
    return default_;
}

void DialogCalibration::onGetXyz(uint8_t type,float x, float y, float z)
{
    if(type != m_dataType)
        return;
    if(!this->isVisible())
        return;
    //
    float x_d = x + m_preOffset[0];
    float y_d = y + m_preOffset[1];
    float z_d = z + m_preOffset[2];
    //
    if(m_isSampling && m_sampleCountdown > 0)
    {
        m_sampleCountdown --;
        if(m_sampleCountdown <= 0)
        {
            ui->bt_input->setText(QString::fromUtf8("开始采集"));
            ui->samplesPerTime->setEnabled(true);
        }
        //
        ui->sample->append(QString().sprintf("%g %g %g",x_d,y_d,z_d));
        emit ui->view->addPoint(x_d,y_d,z_d);
    }
    //
    m_xyzDisplay[0] = m_xyzDisplay[0] * 0.95 + x_d * 0.05;
    m_xyzDisplay[1] = m_xyzDisplay[1] * 0.95 + y_d * 0.05;
    m_xyzDisplay[2] = m_xyzDisplay[2] * 0.95 + z_d * 0.05;
    ui->display_x->setText(QString("%1").arg(m_xyzDisplay[0]));
    ui->display_y->setText(QString("%1").arg(m_xyzDisplay[1]));
    ui->display_z->setText(QString("%1").arg(m_xyzDisplay[2]));
    //
    emit ui->view->onDirectionChanged(x_d,y_d,z_d);
}

void DialogCalibration::on_bt_input_clicked()
{
    if(m_sampleCountdown <= 0)
    {
        // 开始采集。
        ui->bt_input->setText(QString::fromUtf8("停止采集"));
        ui->samplesPerTime->setEnabled(false);
        //
        if(! m_isSampling)
        {
            m_isSampling = true;
            ui->preOffset->setEnabled(false);
            //
            m_preOffset[0] = getFloatFromLineEdit(ui->preOffset_x,0);
            m_preOffset[1] = getFloatFromLineEdit(ui->preOffset_y,0);
            m_preOffset[2] = getFloatFromLineEdit(ui->preOffset_z,0);
        }
        //
        m_sampleCountdown = getUintFromLineEdit(ui->samplesPerTime,16);
    }
    else
    {
        // 停止采集。
        ui->bt_input->setText(QString::fromUtf8("开始采集"));
        ui->samplesPerTime->setEnabled(true);
        //
        m_sampleCountdown = 0;
    }
}

void DialogCalibration::on_bt_clear_clicked()
{
    m_sampleCountdown = 0;
    m_isSampling = false;
    //
    ui->sample->clear();
    ui->view->clearPoints();
    //
    ui->preOffset->setEnabled(true);
    ui->bt_input->setEnabled(true);
    ui->samplesPerTime->setEnabled(true);
}

void DialogCalibration::on_bt_calculate_clicked()
{
    Fitting ft;
    //
    QString samples = ui->sample->toPlainText();
    QTextStream ts(&samples);
    while(! ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList sl = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(sl.size() < 3)
            continue;
        //
        bool ok = false;
        float x = sl[0].toFloat(&ok);
        if(! ok)
            continue;
        float y = sl[1].toFloat(&ok);
        if(! ok)
            continue;
        float z = sl[2].toFloat(&ok);
        if(! ok)
            continue;
        //
        ft.input(x,y,z);
    }
    //
    m_radius = getFloatFromLineEdit(ui->radius,1);
    ft.calculate(m_radius);
    //
    const double * rs = ft.getSolution();
    for(int i=0;i<3;i++)
    {
        m_resultScale[i] = (float)(rs[i]);
        m_resultOffset_noPre[i] = (float)(rs[i+3]/rs[i]);
    }
    //
    // 检查结果有效性。
    bool badResult = false;
    for(int i=0;i<3;i++)
        if(isnan(m_resultScale[i]) || isnan(m_resultOffset_noPre[i]))
            badResult = true;
    if(badResult)
    {
        ui->result->setText(QString::fromUtf8("样本不良。"));
        return;
    }
    //
    // 显示结果。
    QString str,tmp;
    str.append("X_OFFSET = ");
    str.append(tmp.sprintf("%g",m_resultOffset_noPre[0] + m_preOffset[0]));
    str.append(";\n");
    str.append("Y_OFFSET = ");
    str.append(tmp.sprintf("%g",m_resultOffset_noPre[1] + m_preOffset[1]));
    str.append(";\n");
    str.append("Z_OFFSET = ");
    str.append(tmp.sprintf("%g",m_resultOffset_noPre[2] + m_preOffset[2]));
    str.append(";\n");
    //
    str.append("X_GAIN   = ");
    str.append(tmp.sprintf("%g",m_resultScale[0]));
    str.append(";\n");
    str.append("Y_GAIN   = ");
    str.append(tmp.sprintf("%g",m_resultScale[1]));
    str.append(";\n");
    str.append("Z_GAIN   = ");
    str.append(tmp.sprintf("%g",m_resultScale[2]));
    str.append(";");
    ui->result->setText(str);
    //
    // 刷新图形。
    emit ui->view->setEllipsoid(m_resultOffset_noPre[0],m_resultOffset_noPre[1],
        m_resultOffset_noPre[2],m_radius/m_resultScale[0],
        m_radius/m_resultScale[1],m_radius/m_resultScale[2]);
}

void DialogCalibration::on_bt_refleshDisplay_clicked()
{
    ui->view->clearPoints();
    //
    QString samples = ui->sample->toPlainText();
    QTextStream ts(&samples);
    while(! ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList sl = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(sl.size() < 3)
            continue;
        //
        bool ok = false;
        float x = sl[0].toFloat(&ok);
        if(! ok)
            continue;
        float y = sl[1].toFloat(&ok);
        if(! ok)
            continue;
        float z = sl[2].toFloat(&ok);
        if(! ok)
            continue;
        //
        emit ui->view->addPoint(x,y,z);
    }
}

void DialogCalibration::on_displayEllipsoid_stateChanged(int arg1)
{
    if(arg1)
    {
        emit ui->view->needDrawEllipsoid(true);
    }
    else
        emit ui->view->needDrawEllipsoid(false);
}

void DialogCalibration::on_btCheckValue_clicked()
{
    ui->textCheck->clear();
    //
    QString samples = ui->sample->toPlainText();
    QTextStream ts(&samples);
    while(! ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList sl = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(sl.size() < 3)
            continue;
        //
        bool ok = false;
        float x = sl[0].toFloat(&ok);
        if(! ok)
            continue;
        float y = sl[1].toFloat(&ok);
        if(! ok)
            continue;
        float z = sl[2].toFloat(&ok);
        if(! ok)
            continue;
        //
        float x_c = (x + m_resultOffset_noPre[0]) * m_resultScale[0];
        float y_c = (y + m_resultOffset_noPre[1]) * m_resultScale[1];
        float z_c = (z + m_resultOffset_noPre[2]) * m_resultScale[2];
        //
        ui->textCheck->append(QString("%1 %2 %3").arg(x_c).arg(y_c).arg(z_c));
    }
}

void DialogCalibration::on_btCheckLength_clicked()
{
    ui->textCheck->clear();
    ui->textCheck->append(QString::fromUtf8("长度      误差百分数"));
    //
    QString samples = ui->sample->toPlainText();
    QTextStream ts(&samples);
    QString tmp;
    while(! ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList sl = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(sl.size() < 3)
            continue;
        //
        bool ok = false;
        float x = sl[0].toFloat(&ok);
        if(! ok)
            continue;
        float y = sl[1].toFloat(&ok);
        if(! ok)
            continue;
        float z = sl[2].toFloat(&ok);
        if(! ok)
            continue;
        //
        float x_c = (x + m_resultOffset_noPre[0]) * m_resultScale[0];
        float y_c = (y + m_resultOffset_noPre[1]) * m_resultScale[1];
        float z_c = (z + m_resultOffset_noPre[2]) * m_resultScale[2];
        float len_c = sqrtf(x_c*x_c + y_c*y_c + z_c*z_c);
        float deviation = 100 * (len_c - m_radius) / m_radius;
        //
        ui->textCheck->append(tmp.sprintf("%-10g %g%%",len_c,deviation));
    }
}

void DialogCalibration::on_autoGetData_stateChanged(int arg1)
{
    if(arg1)
    {
        //
        m_timerAutoGetData.setInterval(getUintFromLineEdit(ui->autoGetDataInterval,50));
        m_timerAutoGetData.start();
    }
    else
    {
        m_timerAutoGetData.stop();
    }
    //
    ui->autoGetDataInterval->setEnabled(! arg1);
}

void DialogCalibration::onAutoGetDataTimer()
{
    emit getXyz((uint8_t)m_dataType);
}

void DialogCalibration::setupDataOption()
{
    int type[] =
    {
        Protocol::VectorType::RAW_ACC,
        Protocol::VectorType::CALIBRATED_ACC,
        Protocol::VectorType::RAW_GYR,
        Protocol::VectorType::CALIBRATED_GYR,
        Protocol::VectorType::RAW_MAG,
        Protocol::VectorType::CALIBRATED_MAG,
    };
    const char * text[] =
    {
        "加速度计原始值",
        "加速度计校正值",
        "陀螺仪原始值",
        "陀螺仪校正值",
        "罗盘原始值",
        "罗盘校正值",
    };
    //
    for(int i=0;i<6;i++)
    {
        ui->dataType->addItem(QString::fromUtf8(text[i]),QVariant(type[i]));
    }
}

void DialogCalibration::on_dataType_currentIndexChanged(int index)
{
    bool ok = false;
    int type = ui->dataType->itemData(index).toInt(&ok);
    if(ok)
        m_dataType = type;
}

void DialogCalibration::on_btMakeGyrOffset_clicked()
{
    float sx = 0;
    float sy = 0;
    float sz = 0;
    int32_t count = 0;
    //
    QString samples = ui->sample->toPlainText();
    QTextStream ts(&samples);
    while(! ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList sl = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(sl.size() < 3)
            continue;
        //
        bool ok = false;
        float x = sl[0].toFloat(&ok);
        if(! ok)
            continue;
        float y = sl[1].toFloat(&ok);
        if(! ok)
            continue;
        float z = sl[2].toFloat(&ok);
        if(! ok)
            continue;
        //
        sx += x;
        sy += y;
        sz += z;
        count ++;
    }
    //
    if(count != 0)
    {
        sx /= count;
        sy /= count;
        sz /= count;
    }
    sx -= m_preOffset[0];
    sy -= m_preOffset[1];
    sz -= m_preOffset[2];
    //
    ui->leGyrAverage->setText(QString("%1 %2 %3").arg(sx).arg(sy).arg(sz));
    QVector<float> offset(3);
    offset[0] = -sx; // 偏移校正等于偏移值的相反数。
    offset[1] = -sy;
    offset[2] = -sz;
    emit makeGyrOffset(offset);
}

void DialogCalibration::on_btMakeAccParam_clicked()
{
    QVector<float> param;
    for(int i=0;i< 3;i++)
        param.append(m_resultOffset_noPre[i] + m_preOffset[i]);
    for(int i=0;i< 3;i++)
        param.append(m_resultScale[i]);
    //
    emit makeAccParam(param);
}

void DialogCalibration::on_btMakeMagParam_clicked()
{
    QVector<float> param;
    for(int i=0;i< 3;i++)
        param.append(m_resultOffset_noPre[i] + m_preOffset[i]);
    for(int i=0;i< 3;i++)
        param.append(m_resultScale[i]);
    //
    emit makeMagParam(param);
}
