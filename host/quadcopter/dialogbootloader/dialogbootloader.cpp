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

#include "dialogbootloader.h"
#include "ui_dialogbootloader.h"
#include "protocol.h"

static const char * SETTING_BOOTLOADER_FILE_PATH = "bootloader/file_path";

DialogBootloader::DialogBootloader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBootloader),
    m_status(Status::INIT)
{
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
    //
    QSettings settings;
    ui->leFilePath->setText
            (settings.value(SETTING_BOOTLOADER_FILE_PATH,"").toString());
    //
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    m_timer.setInterval(20);
}

DialogBootloader::~DialogBootloader()
{
    delete ui;
}

void DialogBootloader::resizeEvent(QResizeEvent *event)
{
    // 默认布局大小不会自动变，要手动设置。
    QSize s = event->size();
    QRect r(0,0,s.width(),s.height());
    ui->horizontalLayoutWidget->setGeometry(r);
}

void DialogBootloader::on_btSelectFile_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName
            (this, QString::fromUtf8("选择App固件"),
             ui->leFilePath->text(),
             QString::fromUtf8("*.bin (bin固件);;*.* (*.*)"));
    if (fileName.isEmpty())
        return;
    //
    ui->leFilePath->setText(fileName);
    ui->leFilePath->setToolTip(fileName);
    //
    QSettings settings;
    settings.setValue(SETTING_BOOTLOADER_FILE_PATH,fileName);
}

void DialogBootloader::on_btStartRefreshApp_clicked()
{
    if(m_status == Status::INIT)
    {
        if(m_fileReader.readFile(ui->leFilePath->text()) != 0)
        {
            QMessageBox::warning(this,QString::fromUtf8("读取文件失败"),
                                 QString::fromUtf8("文件不存在或没有读取权限"));
            return;
        }
        //
        m_status = Status::ENTERING_BOOTLOADER;
        ui->btStartRefreshApp->setText(QString::fromUtf8("停止更新"));
        ui->btSelectFile->setEnabled(false);
        ui->leFilePath->setEnabled(false);
        //
        ui->progressBar->setValue(0);
        ui->teOutput->clear();
        m_timer.start();
    }
    else
    {
        m_status = Status::INIT;
        ui->btStartRefreshApp->setText(QString::fromUtf8("更新程序"));
        ui->btSelectFile->setEnabled(true);
        ui->leFilePath->setEnabled(true);
        //
        m_timer.stop();
        ui->teOutput->append(QString::fromUtf8("用户中断更新。"));
    }
}

void DialogBootloader::onTimer()
{
    switch(m_status)
    {
    case Status::ENTERING_BOOTLOADER :
        emit cmd(Protocol::BootloaderCmd::ENTER_BOOTLOADER,QByteArray());
        break;
    case Status::BOOTLOADER_READY :
        emit cmd(Protocol::BootloaderCmd::BEGIN_REFRESH_APP,QByteArray());
        break;
    case Status::READY_TO_REFRESH_APP :
        sendLengthCrc();
        break;
    case Status::WRITING_APP :
        writeSomeBlock();
        break;
    case Status::REFRESH_APP_SUCCEED :
        emit cmd(Protocol::BootloaderCmd::ENTER_APP,QByteArray());
        refreshComplete(true);
        break;
    case Status::REFRESH_APP_FAILED :
        refreshComplete(false);
        break;
    case Status::RUNNING_APP :
        m_timer.stop();
        break;
    }
}

void DialogBootloader::onStatus(uint8_t status, const QByteArray param)
{
    switch(status)
    {
    case Protocol::BootloaderStatus::STAY_AT_BOOTLOADER :
        if(m_status != Status::ENTERING_BOOTLOADER)
            break;
        m_status = Status::BOOTLOADER_READY;
        ui->teOutput->append(QString::fromUtf8("已经进入Bootloader模式。"));
        break;
    case Protocol::BootloaderStatus::READY_TO_REFRESH_APP :
        if(m_status != Status::BOOTLOADER_READY)
            break;
        m_status = Status::READY_TO_REFRESH_APP;
        ui->teOutput->append(QString::fromUtf8("下位机已经准备好刷程序。"));
        break;
    case Protocol::BootloaderStatus::WRITING_APP :
        if(m_status == Status::READY_TO_REFRESH_APP)
        {
            // 准备写。
            m_status = Status::WRITING_APP;
            ui->teOutput->append(QString::fromUtf8("开始写入程序。"));
        }
        else if(m_status == Status::WRITING_APP)
        {
            // 正在写。
            onWriting(param);
        }
        break;
    case Protocol::BootloaderStatus::BLOCK_REMAIN :
        if(m_status != Status::WRITING_APP)
            break;
        onRemainRegion(param);
        break;
    case Protocol::BootloaderStatus::WRITE_APP_SUCCEED :
        if(m_status != Status::WRITING_APP)
            break;
        m_status = Status::REFRESH_APP_SUCCEED;
        ui->teOutput->append(QString::fromUtf8("程序更新完毕，即将运行新程序"));
        emit cmd(Protocol::BootloaderCmd::ENTER_APP,QByteArray());
        ui->progressBar->setValue(ui->progressBar->maximum());
        break;
    case Protocol::BootloaderStatus::APP_CRC_ERROR :
        if(m_status != Status::WRITING_APP)
            break;
        m_status = Status::REFRESH_APP_FAILED;
        ui->teOutput->append(QString::fromUtf8("程序更新失败。请重新尝试。"));
        ui->progressBar->setValue(ui->progressBar->maximum());
        break;
    }
}

int DialogBootloader::sendLengthCrc()
{
    uint32_t len = 0;
    uint32_t crc = 0;
    if(m_fileReader.getLengthCrc(&len,&crc) != 0)
        return -1;
    //
    QByteArray ba;
    for(int i=0;i<4;i++)
        ba.append(((char *)&len)[i]);
    for(int i=0;i<4;i++)
        ba.append(((char *)&crc)[i]);
    emit cmd(Protocol::BootloaderCmd::APP_LENGTH_CRC,ba);
    //
    return 0;
}

int DialogBootloader::onWriting(const QByteArray &param)
{
    if(param.size() < 4)
        return -1;
    //
    uint32_t blocksWritten = 0;
    for(int i=0;i<4;i++)
        ((uint8_t *)&blocksWritten)[i] = param[i];
    //
    int value = ui->progressBar->maximum() * blocksWritten / m_fileReader.getBlocksAmount();
    ui->progressBar->setValue(value);
    //
    return 0;
}

int DialogBootloader::onRemainRegion(const QByteArray &raw)
{
    int len = raw.size() / 4;
    if(len == 0 && m_fileReader.isNoMoreblock())
        return -1;
    //
    for(int i=0;i<len;i++)
    {
        uint16_t index = 0;
        uint16_t length = 0;
        ((uint8_t *)&index)[0] = raw[i*4 + 0];
        ((uint8_t *)&index)[1] = raw[i*4 + 1];
        ((uint8_t *)&length)[0] = raw[i*4 + 2];
        ((uint8_t *)&length)[1] = raw[i*4 + 3];
        //
        m_fileReader.addRegion(index,length);
    }
    //
    return 0;
}

void DialogBootloader::refreshComplete(bool isSucceed)
{
    m_status = Status::INIT;
    ui->btStartRefreshApp->setText(QString::fromUtf8("更新程序"));
    ui->btSelectFile->setEnabled(true);
    ui->leFilePath->setEnabled(true);
    m_timer.stop();
}

void DialogBootloader::writeSomeBlock()
{
    static int wait_times = 0;
    if(wait_times > 0)
    {
        wait_times --;
        return;
    }
    //
    QByteArray send;
    for(int i=0;i<6;i++)
    {
        if(m_fileReader.getNextBlocks(&send,26) != 0
                || send.size() <= 0)
        {
            emit cmd(Protocol::BootloaderCmd::QUEST_REMAIN_BLOCK,QByteArray());
            wait_times = 5;
            break;
        }
        else
        {
            emit cmd(Protocol::BootloaderCmd::WRITE_APP,send);
        }
    }
}
