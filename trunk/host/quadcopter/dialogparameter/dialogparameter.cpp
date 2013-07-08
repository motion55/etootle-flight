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

#include "dialogparameter.h"
#include "ui_dialogparameter.h"
#include "protocol.h"
#include <QMessageBox>
#include "itemdelegate.h"

DialogParameter::DialogParameter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogParameter)
{
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    //
    ui->wExtend->hide();
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

void DialogParameter::setupAreas()
{
    const char * paramNameList[] =
    {
        "加计偏移_X",
        "加计偏移_Y",
        "加计偏移_Z",
        "加计比例_X",
        "加计比例_Y",
        "加计比例_Z",
        //
        "陀螺偏移_X",
        "陀螺偏移_Y",
        "陀螺偏移_Z",
        "陀螺比例_X",
        "陀螺比例_Y",
        "陀螺比例_Z",
        //
        "罗盘偏移_X",
        "罗盘偏移_Y",
        "罗盘偏移_Z",
        "罗盘比例_X",
        "罗盘比例_Y",
        "罗盘比例_Z",
        //
        "水平校正_W",
        "水平校正_X",
        "水平校正_Y",
        "水平校正_Z",
        //
        "X轴PID_P",
        "X轴PID_I",
        "X轴PID_D",
        "Y轴PID_P",
        "Y轴PID_I",
        "Y轴PID_D",
        "Z轴PID_P",
        "Z轴PID_I",
        "Z轴PID_D",
        //
        "停机电压(V)",
        "停机倾角(cos)"
    };
    //
    m_tableViewModel.setHorizontalHeaderItem(COLUMN_INDEX_NAME  ,new QStandardItem(QString::fromUtf8("参数名")));
    m_tableViewModel.setHorizontalHeaderItem(COLUMN_INDEX_GET   ,new QStandardItem(QString::fromUtf8("获取值")));
    m_tableViewModel.setHorizontalHeaderItem(COLUMN_INDEX_SET   ,new QStandardItem(QString::fromUtf8("修改值")));
    m_tableViewModel.setHorizontalHeaderItem(COLUMN_INDEX_STATUS,new QStandardItem(QString::fromUtf8("状态")));
    ui->tableView->setColumnWidth(COLUMN_INDEX_NAME  ,128);
    ui->tableView->setColumnWidth(COLUMN_INDEX_GET   ,128);
    ui->tableView->setColumnWidth(COLUMN_INDEX_SET   ,128);
    ui->tableView->setColumnWidth(COLUMN_INDEX_STATUS,64);
    ui->tableView->setItemDelegateForColumn(COLUMN_INDEX_NAME  ,new ItemDelegate::ReadOnlyDelegate(this));
    ui->tableView->setItemDelegateForColumn(COLUMN_INDEX_GET   ,new ItemDelegate::ReadOnlyDelegate(this));
    ui->tableView->setItemDelegateForColumn(COLUMN_INDEX_SET   ,new ItemDelegate::FloatValueDelegate(this));
    ui->tableView->setItemDelegateForColumn(COLUMN_INDEX_STATUS,new ItemDelegate::ReadOnlyDelegate(this));
    //
    for(int i=0;i<int(sizeof(paramNameList)/sizeof(paramNameList[0]));i++)
    {
        m_rows.append(QString::fromUtf8(paramNameList[i]));
        //
        m_tableViewModel.setVerticalHeaderItem(i,new QStandardItem(QString("%1").arg(i+1)));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_NAME  ),QVariant(m_rows[i]));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_GET   ),QVariant(""));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_SET   ),QVariant(""));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_STATUS),
                                 QVariant(QString::fromUtf8("未读取")));
    }
}

void DialogParameter::on_btGetParameter_clicked()
{
    QPair<int,int> selection = getSelections();
    if(selection.second < selection.first)
        return;
    //
    for(int i=selection.first;i<=selection.second;i++)
        changeRowStatus(i,QString::fromUtf8("正在读取"));
    //
    QByteArray p;
    p.append(Protocol::Parameter::GET_RAM);
    p.append(selection.first+1); // 行索引转位置，需要加1。
    p.append(selection.second - selection.first + 1); // 长度=行数。
    emit parameter(p);
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
                //
                int row = pos-1 + i; // 位置减1才是行索引。
                m_tableViewModel.setData(m_tableViewModel.index(row,COLUMN_INDEX_GET),
                                         QVariant(QString("%1").arg(value)));
                changeRowStatus(row,QString::fromUtf8("已读取"));
            }
        }
        break;
    }
}

void DialogParameter::onMakeGyrOffset(QVector<float> offset)
{
    for(int i=0;i<3;i++)
    {
        m_tableViewModel.setData(m_tableViewModel.index(6+i,COLUMN_INDEX_SET),
                                 QVariant(QString("%1").arg(offset[i])));
    }
}

void DialogParameter::onMakeAccParam(QVector<float> param)
{
    for(int i=0;i<6;i++)
    {
        m_tableViewModel.setData(m_tableViewModel.index(0+i,COLUMN_INDEX_SET),
                                 QVariant(QString("%1").arg(param[i])));
    }
}

void DialogParameter::onMakeMagParam(QVector<float> param)
{
    for(int i=0;i<6;i++)
    {
        m_tableViewModel.setData(m_tableViewModel.index(12+i,COLUMN_INDEX_SET),
                                 QVariant(QString("%1").arg(param[i])));
    }
}

void DialogParameter::onMakeHorizontalAttitude(Quaternion q)
{
    for(int i=0;i<4;i++)
    {
        m_tableViewModel.setData(m_tableViewModel.index(18+i,COLUMN_INDEX_SET),
                                 QVariant(QString("%1").arg(q.value()[i])));
    }
}

void DialogParameter::on_btSetParameter_clicked()
{
    QPair<int,int> selection = getSelections();
    if(selection.second < selection.first)
        return;
    //
    QString msg = QString::fromUtf8("确定要修改吗？");
    if(QMessageBox::question(this,QString::fromUtf8("确认操作"),msg,
             QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)
            != QMessageBox::Ok)
        return;
    //
    uint8_t pos = static_cast<uint8_t>(selection.first+1); // 行索引转参数位置，要加1。
    QVector<int32_t> value;
    for(int row=selection.first;row<=selection.second;row++)
    {
        bool ok = false;
        float v_f = m_tableViewModel.data(
                    m_tableViewModel.index(row,COLUMN_INDEX_SET)).toString().toFloat(&ok);
        if(! ok)
        {
            QMessageBox::critical(this,QString::fromUtf8("错误"),
                                  QString::fromUtf8("遇到错误的值，修改操作已取消。"));
            return;
        }
        //
        value.append(*(int32_t *)(void *)&v_f);
    }
    //
    for(int i=selection.first;i<=selection.second;i++)
        changeRowStatus(i,QString::fromUtf8("正在写入"));
    setParameter(pos,value);
}

QPair<int, int> DialogParameter::getSelections()
{
    const QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();
    //
    int start = 999;
    int end = -1;
    foreach(QModelIndex index, selection)
    {
        int row = index.row();
        if(row < start)
            start = row;
        if(row > end)
            end = row;
    }
    //
    return QPair<int,int>(start,end);
}

void DialogParameter::changeRowStatus(int row, QString status)
{
    m_tableViewModel.setData(m_tableViewModel.index(row,COLUMN_INDEX_STATUS),QVariant(status));
}

void DialogParameter::setParameter(uint8_t pos, const QVector<int32_t> &value)
{
    const static int MAX_LENGTH_PER_PACKET = 6;
    int fullPacketAmount = value.size() / MAX_LENGTH_PER_PACKET;
    const int32_t * v = value.constData();
    int offset = 0;
    for(int i=0;i<fullPacketAmount;i++)
    {
        QByteArray p;
        p.append(Protocol::Parameter::SET_RAM);
        p.append(static_cast<uint8_t>(pos + offset));
        p.append((const char *)(&v[offset]),MAX_LENGTH_PER_PACKET*4);
        emit parameter(p);
        //
        offset += MAX_LENGTH_PER_PACKET;
    }
    if(offset < value.size())
    {
        QByteArray p;
        p.append(Protocol::Parameter::SET_RAM);
        p.append(static_cast<uint8_t>(pos + offset));
        p.append((const char *)(&v[offset]),(value.size()-offset)*4);
        emit parameter(p);
    }
}

void DialogParameter::on_btClear_clicked()
{
    for(int i=0;i<m_rows.size();i++)
    {
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_GET),QVariant(""));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_SET),QVariant(""));
        m_tableViewModel.setData(m_tableViewModel.index(i,COLUMN_INDEX_STATUS)
                                 ,QVariant(QString::fromUtf8("未读取")));
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

void DialogParameter::on_btShowHideExtend_clicked()
{
    if(ui->wExtend->isHidden())
    {
        ui->btShowHideExtend->setText(QString::fromUtf8("↑"));
        ui->wExtend->show();
    }
    else
    {
        ui->btShowHideExtend->setText(QString::fromUtf8("↓"));
        ui->wExtend->hide();
    }
}

void DialogParameter::on_btMixHorizontalAttitude_clicked()
{
    const static int HORIZONTAL_ATTITUDE_ROW_INDEX = 18;
    //
    QVector<int32_t> value;
    for(int row=HORIZONTAL_ATTITUDE_ROW_INDEX;row<HORIZONTAL_ATTITUDE_ROW_INDEX+4;row++)
    {
        bool ok = false;
        float v_f = m_tableViewModel.data(
                    m_tableViewModel.index(row,COLUMN_INDEX_SET)).toString().toFloat(&ok);
        if(! ok)
        {
            QMessageBox::critical(this,QString::fromUtf8("错误"),
                                  QString::fromUtf8("遇到错误的值，修改操作已取消。"));
            return;
        }
        //
        value.append(*(int32_t *)(void *)&v_f);
    }
    //
    QString msg = QString::fromUtf8("确定要把水平校正修改值叠加到现有值吗？");
    if(QMessageBox::question(this,QString::fromUtf8("确认操作"),msg,
             QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)
            != QMessageBox::Ok)
        return;
    //
    for(int row=HORIZONTAL_ATTITUDE_ROW_INDEX;row<HORIZONTAL_ATTITUDE_ROW_INDEX+4;row++)
        changeRowStatus(row,QString::fromUtf8("正在修改"));
    //
    QByteArray p;
    p.append(Protocol::Parameter::OPERATE_RAM);
    p.append(Protocol::Parameter::OperateRam::MIX_HORIZONTAL_ATTITUDE);
    p.append((char *)value.constData(),value.size()*4);
    emit parameter(p);
}
