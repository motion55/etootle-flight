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

#include "dialogparameter.h"
#include "ui_dialogparameter.h"
#include "protocol.h"
#include <QMessageBox>

DialogParameter::DialogParameter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogParameter)
{
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    //
    ui->tableView->setModel(&m_tableViewModel);
    setupAreas();
}

DialogParameter::~DialogParameter()
{
    delete ui;
}

void DialogParameter::resizeEvent(QResizeEvent * event)
{
    // 默认布局大小不会自动变，要手动设置。
    QSize s = event->size();
    QRect r(0,0,s.width(),s.height());
    ui->horizontalLayoutWidget->setGeometry(r);
}

float DialogParameter::getFloat(QLineEdit *from, float default_)
{
    bool ok = true;
    float v = from->text().toFloat(&ok);
    //
    // 成功就返回。
    if(ok)
        return v;
    //
    // 失败就重设文本。
    from->setText(QString().sprintf("%g",default_));
    return default_;
}

void DialogParameter::setupAreas()
{
    Area sensor;
    sensor.m_leftIndex = 0;
    sensor.m_topIndex = 0;
    sensor.m_posBase = 1;
    sensor.m_groupName = QString::fromUtf8("传感器校正值");
    sensor.m_verticalList.append(QString::fromUtf8("加计偏移:"));
    sensor.m_verticalList.append(QString::fromUtf8("加计比例:"));
    sensor.m_verticalList.append(QString::fromUtf8("陀螺偏移:"));
    sensor.m_verticalList.append(QString::fromUtf8("陀螺比例:"));
    sensor.m_verticalList.append(QString::fromUtf8("罗盘偏移:"));
    sensor.m_verticalList.append(QString::fromUtf8("罗盘比例:"));
    sensor.m_horizontalList.append(QString::fromUtf8("X"));
    sensor.m_horizontalList.append(QString::fromUtf8("Y"));
    sensor.m_horizontalList.append(QString::fromUtf8("Z"));
    m_areas.append(sensor);
    //
    Area ctrlPid;
    ctrlPid.m_leftIndex = sensor.getRightIndex() + 2;
    ctrlPid.m_topIndex = sensor.m_topIndex;
    ctrlPid.m_posBase = 23;
    ctrlPid.m_groupName = QString::fromUtf8("控制PID参数");
    ctrlPid.m_verticalList.append(QString::fromUtf8("X(roll):"));
    ctrlPid.m_verticalList.append(QString::fromUtf8("Y(pitch):"));
    ctrlPid.m_verticalList.append(QString::fromUtf8("Z(yaw):"));
    ctrlPid.m_horizontalList.append(QString::fromUtf8("P"));
    ctrlPid.m_horizontalList.append(QString::fromUtf8("I"));
    ctrlPid.m_horizontalList.append(QString::fromUtf8("D"));
    m_areas.append(ctrlPid);
    //
    Area horizonAttitude;
    horizonAttitude.m_leftIndex = sensor.m_leftIndex;
    horizonAttitude.m_topIndex = sensor.getBottomIndex() + 2;
    horizonAttitude.m_posBase = 19;
    horizonAttitude.m_groupName = QString::fromUtf8("水平校正值");
    horizonAttitude.m_verticalList.append(QString::fromUtf8("水平姿态:"));
    horizonAttitude.m_horizontalList.append(QString::fromUtf8("w"));
    horizonAttitude.m_horizontalList.append(QString::fromUtf8("x"));
    horizonAttitude.m_horizontalList.append(QString::fromUtf8("y"));
    horizonAttitude.m_horizontalList.append(QString::fromUtf8("z"));
    m_areas.append(horizonAttitude);
    //
    // 显示。
    int row = 0;
    int col = 0;
    for(int i=0;i<m_areas.size();i++)
    {
        if(m_areas[i].getRightIndex() > col)
            col = m_areas[i].getRightIndex();
        if(m_areas[i].getBottomIndex() > row)
            row = m_areas[i].getBottomIndex();
    }
    for(int i=0;i<=row;i++)
        m_tableViewModel.setVerticalHeaderItem(i,new QStandardItem(""));
    for(int i=0;i<=col;i++)
        m_tableViewModel.setHorizontalHeaderItem(i,new QStandardItem(""));
    for(int i=0;i<m_areas.size();i++)
        m_areas[i].addToModel(m_tableViewModel);
    //
    // 建立映射。
    for(int i=0;i<m_areas.size();i++)
    {
        for(int j=0;j<m_areas[i].m_verticalList.size();j++)
        {
            for(int k=0;k<m_areas[i].m_horizontalList.size();k++)
            {
                int row = m_areas[i].m_topIndex + 1 + j;
                int col = m_areas[i].m_leftIndex + 1 + k;
                int pos = m_areas[i].m_posBase+j*m_areas[i].m_horizontalList.size()+k;
                //
                m_posToRowCol.insert(pos,QPair<int,int>(row,col));
            }
        }
    }
}

void DialogParameter::Area::addToModel(QStandardItemModel &model)
{
    model.setData(model.index(m_topIndex,m_leftIndex),QVariant(m_groupName));
    for(int i=0;i<m_verticalList.size();i++)
        model.setData(model.index(m_topIndex+1+i,m_leftIndex)
                                 ,QVariant(m_verticalList[i]));
    for(int i=0;i<m_horizontalList.size();i++)
        model.setData(model.index(m_topIndex,m_leftIndex+1+i)
                      ,QVariant(m_horizontalList[i]));
}

int DialogParameter::Area::hit(int row, int col)
{
    if(row <= m_topIndex || row > m_topIndex+m_verticalList.size()
            || col <= m_leftIndex || col > m_leftIndex+m_horizontalList.size())
        return 0;
    return m_posBase+(col-m_leftIndex-1)+(row-m_topIndex-1)*m_horizontalList.size();
}

void DialogParameter::on_btGetParameter_clicked()
{
    QModelIndex cur = ui->tableView->currentIndex();
    int pos = 0;
    for(int i=0;i<m_areas.size();i++)
    {
        pos = m_areas[i].hit(cur.row(),cur.column());
        if(pos != 0)
            break;
    }
    if(pos == 0)
        return;
    //
    m_tableViewModel.setData(cur,QVariant(cur.data().toString()+QString(">")));
    getParameter(pos);
}

void DialogParameter::on_btSetParameter_clicked()
{
    QModelIndex cur = ui->tableView->currentIndex();
    int pos = 0;
    int area_index = 0;
    for(area_index=0;area_index<m_areas.size();area_index++)
    {
        pos = m_areas[area_index].hit(cur.row(),cur.column());
        if(pos != 0)
            break;
    }
    if(pos == 0)
        return;
    //
    float value = getFloat(ui->leNewValue);
    QString msg = QString::fromUtf8("确定要把 #%1 设置为 %2 ?")
            .arg(pos)
            .arg(value);
    if(QMessageBox::question(this,QString::fromUtf8("确认操作"),msg,
             QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)
            != QMessageBox::Ok)
        return;
    m_tableViewModel.setData(cur,QVariant(cur.data().toString()+QString("<%1").arg(value)));
    //
    setParameter(pos,value);
}

void DialogParameter::onParameter(const QByteArray &param)
{
    const uint8_t * param_b = (const uint8_t *)param.constData();
    //
    switch(param_b[0])
    {
    case Protocol::Parameter::RETURN_RAM :
        {
            int pos = param_b[1];
            int len = (param.size()-1-1) / 4;
            for(int i=0;i<len;i++)
            {
                float value = ((float *)(param_b+2))[i];
                if(! m_posToRowCol.contains(pos+i))
                    return;
                QPair<int,int> rowCol = m_posToRowCol[pos+i];
                //
                m_tableViewModel.setData(m_tableViewModel.index
                        (rowCol.first,rowCol.second),QVariant(QString("%1").arg(value)));
            }
        }
        break;
    }
}

void DialogParameter::getParameter(int pos)
{
    QByteArray p;
    p.append(Protocol::Parameter::GET_RAM);
    p.append(pos);
    p.append(1);
    emit parameter(p);
}

void DialogParameter::setParameter(int pos, float value)
{
    QByteArray p;
    p.append(Protocol::Parameter::SET_RAM);
    p.append(pos);
    for(int i=0;i<4;i++)
        p.append(((uint8_t *)&value)[i]);
    emit parameter(p);
}

void DialogParameter::on_btClear_clicked()
{
    for(int i=0;i<50;i++)
    {
        if(! m_posToRowCol.contains(i))
            continue;
        QPair<int,int> rowCol = m_posToRowCol[i];
        //
        m_tableViewModel.setData(m_tableViewModel.index
                (rowCol.first,rowCol.second),QVariant(""));
    }
}

void DialogParameter::on_btWriteFlash_clicked()
{
    if(QMessageBox::question(this,QString::fromUtf8("确认操作"),
                             QString::fromUtf8("确定要把RAM里的参数写到Flash吗？"),
                             QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)
            != QMessageBox::Ok)
        return;
    //
    QByteArray p;
    p.append(Protocol::Parameter::WRITE_FLASH);
    emit parameter(p);
}

void DialogParameter::on_btResetFlash_clicked()
{
    if(QMessageBox::question(this,QString::fromUtf8("确认操作"),
                             QString::fromUtf8("确定要重置Flash为默认值吗？"),
                             QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)
            != QMessageBox::Ok)
        return;
    //
    QByteArray p;
    p.append(Protocol::Parameter::RESET_FLASH);
    emit parameter(p);
}
