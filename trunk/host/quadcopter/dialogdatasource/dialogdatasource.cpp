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

#include "dialogdatasource.h"
#include "ui_dialogdatasource.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include "qextserialport/qextserialenumerator.h"

const char * DialogDataSource::SETTING_DATASOURCE_SERIAL_PORT       = "datasource/serial/port";
const char * DialogDataSource::SETTING_DATASOURCE_SERIAL_BOUNDRATE  = "datasource/serial/boundrate";
const char * DialogDataSource::SETTING_DATASOURCE_SERIAL_DATABITS   = "datasource/serial/databits";
const char * DialogDataSource::SETTING_DATASOURCE_SERIAL_PARITY     = "datasource/serial/parity";
const char * DialogDataSource::SETTING_DATASOURCE_SERIAL_STOPBITS   = "datasource/serial/stopbits";
const char * DialogDataSource::SETTING_DATASOURCE_FILE_PATH         = "datasource/file/path";
const char * DialogDataSource::SETTING_DATASOURCE_FILE_PERIOD       = "datasource/file/period";

DialogDataSource::DialogDataSource(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDataSource)
{
    ui->setupUi(this);
    //
    m_file_running = false;
    m_file_pause = false;
    m_serial = 0;
    m_serial_opening = false;
    m_usb = 0;
    m_usb_opening = false;
    //
    m_serial_timer.setInterval(30);
    connect(&m_serial_timer,SIGNAL(timeout()),this,SLOT(every30ms()));
    //
    initFileSetting();
    initSerialSetting();
    //
    connectSignalSlot();
}

DialogDataSource::~DialogDataSource()
{
    if(m_serial_opening)
    {
        m_serial->close();
        delete m_serial;
        m_serial = 0;
    }
    delete ui;
}

BinaryParser &DialogDataSource::getBinaryParser()
{
    return m_serialBinaryParser;
}

TextFileParser &DialogDataSource::getTextParser()
{
    return m_textFileParser;
}

void DialogDataSource::connectSignalSlot()
{
    connect(&m_textFileParser,SIGNAL(onStop()),
            this,SLOT(on_file_stop()));
    //
    // 串口二进制解析器。
    connect(&m_serialBinaryParser,SIGNAL(send(QByteArray)),
            this,SLOT(serial_send(QByteArray)));
}

unsigned int DialogDataSource::getUintFromLineEdit(QLineEdit *from, unsigned int default_)
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

void DialogDataSource::every30ms()
{
    if(m_serial && m_serial->isReadable())
    {
        QByteArray buf = m_serial->readAll();
        emit m_serialBinaryParser.onReceivedData(buf);
    }
    if(m_usb && m_usb->isReadable()){
        QByteArray buf = m_usb->readAll();
        emit m_serialBinaryParser.onReceivedData(buf);
    }
}

void DialogDataSource::initSerialSetting()
{
    typedef struct
    {
        const char * name;
        int value;
    } ComSettingTable;
    static const ComSettingTable baudrateTable[] =
    {
        {"1200bps"  ,BAUD1200},
        {"2400bps"  ,BAUD2400},
        {"4800bps"  ,BAUD4800},
        {"9600bps"  ,BAUD9600},
        {"19200bps" ,BAUD19200},
        {"38400bps" ,BAUD38400},
        {"57600bps" ,BAUD57600},
        {"115200bps",BAUD115200}
    };
    static const ComSettingTable databitsTable[] =
    {
        {"5个数据位",DATA_5},
        {"6个数据位",DATA_6},
        {"7个数据位",DATA_7},
        {"8个数据位",DATA_8}
    };
    static const ComSettingTable parityTable[] =
    {
        {"没有校验" ,PAR_NONE},
        {"奇校验"  ,PAR_ODD},
        {"偶校验"  ,PAR_EVEN},
        {"空校验"  ,PAR_SPACE}
    };
    static const ComSettingTable stopbitsTable[] =
    {
        {"1个停止位"   ,STOP_1},
        {"1.5个停止位" ,STOP_1_5},
        {"2个停止位"   ,STOP_2}
    };
    //
    // 根据不同的平台，判断串口文件名。
#ifdef Q_WS_WIN
    for(int i=1;i<=16;i++)
    {
        QString str;
        if(i < 10)
            str.sprintf("COM%d",i);
        else
            // http://www.fuyunbaike.com/index.php/forum/25-qt/587-qtcom10.html
            str.sprintf("\\\\.\\COM%d",i);
        ui->serial_port->addItem(str);
    }
#else
    for(int i=0;i<8;i++)
    {
        QString str;
        str.sprintf("/dev/ttyUSB%d",i);
        ui->serial_port->addItem(str);
    }
    for(int i=0;i<8;i++)
    {
        QString str;
        str.sprintf("/dev/ttyS%d",i);
        ui->serial_port->addItem(str);
    }
#endif
    on_serial_refresh_clicked();
    on_usb_refresh_clicked();
    //
    ui->serial_baudrate->clear();
    for(int i=0;i<(int)(sizeof(baudrateTable)/sizeof(ComSettingTable));i++)
        ui->serial_baudrate->addItem
                (QString::fromUtf8(baudrateTable[i].name),
                 QVariant(baudrateTable[i].value));
    //
    ui->serial_databits->clear();
    for(int i=0;i<(int)(sizeof(databitsTable)/sizeof(ComSettingTable));i++)
        ui->serial_databits->addItem
                (QString::fromUtf8(databitsTable[i].name),
                 QVariant(databitsTable[i].value));
    //
    ui->serial_parity->clear();
    for(int i=0;i<(int)(sizeof(parityTable)/sizeof(ComSettingTable));i++)
        ui->serial_parity->addItem
                (QString::fromUtf8(parityTable[i].name),
                 QVariant(parityTable[i].value));
    //
    ui->serial_stopbits->clear();
    for(int i=0;i<(int)(sizeof(stopbitsTable)/sizeof(ComSettingTable));i++)
        ui->serial_stopbits->addItem
                (QString::fromUtf8(stopbitsTable[i].name),
                 QVariant(stopbitsTable[i].value));
    //
    QSettings settings;
    ui->serial_port->setCurrentIndex
        (settings.value(SETTING_DATASOURCE_SERIAL_PORT      ,0).toInt());
    ui->serial_baudrate->setCurrentIndex
        (settings.value(SETTING_DATASOURCE_SERIAL_BOUNDRATE ,7).toInt());
    ui->serial_databits->setCurrentIndex
        (settings.value(SETTING_DATASOURCE_SERIAL_DATABITS  ,3).toInt());
    ui->serial_parity->setCurrentIndex
        (settings.value(SETTING_DATASOURCE_SERIAL_PARITY    ,0).toInt());
    ui->serial_stopbits->setCurrentIndex
        (settings.value(SETTING_DATASOURCE_SERIAL_STOPBITS  ,0).toInt());
}

void DialogDataSource::initFileSetting()
{
    QSettings settings;
    ui->file_path->setText
        (settings.value(SETTING_DATASOURCE_FILE_PATH    ,"").toString());
    ui->file_period->setText(QString("%1").arg
        (settings.value(SETTING_DATASOURCE_FILE_PERIOD  ,30).toInt()));
}

void DialogDataSource::dataSorce_getSerialSetting(PortSettings & setting)
{
    int port = ui->serial_port->currentIndex();
    int boundrate = ui->serial_baudrate->currentIndex();
    int databits =  ui->serial_databits->currentIndex();
    int parity =    ui->serial_parity->currentIndex();
    int stopbits =  ui->serial_stopbits->currentIndex();
    //
    QSettings settings;
    settings.setValue(SETTING_DATASOURCE_SERIAL_PORT        ,port);
    settings.setValue(SETTING_DATASOURCE_SERIAL_BOUNDRATE   ,boundrate);
    settings.setValue(SETTING_DATASOURCE_SERIAL_DATABITS    ,databits);
    settings.setValue(SETTING_DATASOURCE_SERIAL_PARITY      ,parity);
    settings.setValue(SETTING_DATASOURCE_SERIAL_STOPBITS    ,stopbits);
    //
    setting.BaudRate = (BaudRateType) ui->serial_baudrate->itemData(boundrate).toInt();
    setting.DataBits = (DataBitsType) ui->serial_databits->itemData(databits).toInt();
    setting.Parity   = (ParityType)   ui->serial_parity->itemData(parity).toInt();
    setting.StopBits = (StopBitsType) ui->serial_stopbits->itemData(stopbits).toInt();
    //
    setting.FlowControl = FLOW_OFF;
    //setting.Timeout_Sec = 0;
    setting.Timeout_Millisec = 20;
}

void DialogDataSource::on_serial_openClose_clicked()
{
    if(!m_serial_opening)
    {
        m_serial_opening = true;
        //
        // 打开串口。
        PortSettings settings;
        dataSorce_getSerialSetting(settings);
        QString portName = ui->serial_port->currentText();
        m_serial = new QextSerialPort(portName,settings);
        if(!m_serial->open(QIODevice::ReadWrite))
        {
            delete m_serial;
            m_serial = 0;
            m_serial_opening = false;
            //
            QMessageBox::warning(this,QString::fromUtf8("错误"),
                                 QString::fromUtf8("打开串口%1失败！").arg(portName));
            return;
        }
        m_serial->flush();
        m_serial_timer.start();
        //
        // 界面。
        ui->serial_openClose->setText(QString::fromUtf8("关闭串口"));
        ui->serial_setting->setEnabled(!m_serial_opening);
        ui->file->setEnabled(!m_serial_opening);
        ui->usb->setEnabled(!m_serial_opening);
    }
    else
    {
        m_serial_opening = false;
        //
        m_serial_timer.stop();
        m_serial->close();
        delete m_serial;
        m_serial = 0;
        //
        // 界面。
        ui->serial_openClose->setText(QString::fromUtf8("打开串口"));
        ui->serial_setting->setEnabled(!m_serial_opening);
        ui->file->setEnabled(!m_serial_opening);
        ui->usb->setEnabled(!m_serial_opening);
    }
}

void DialogDataSource::on_serial_checkCrc_stateChanged(int arg1)
{
    m_serialBinaryParser.needCheckCrc(arg1);
}

void DialogDataSource::on_file_selectFile_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName
            (this, QString::fromUtf8("选择记录文件"),NULL, "*.* (*.*)");
    if (fileName.isEmpty())
        return;
    //
    ui->file_path->setText(fileName);
    ui->file_path->setToolTip(fileName);
}

void DialogDataSource::on_file_startStop_clicked()
{
    if(m_file_running)
    {
        m_textFileParser.stop();
    }
    else
    {
        // 检查设置，打开文件，启动计时器。
        int period = getUintFromLineEdit(ui->file_period,30);
        if(! m_textFileParser.start(ui->file_path->text(),period))
        {
            QMessageBox::critical(this,QString::fromUtf8("错误"),
                    QString::fromUtf8("打开文件%1失败").arg(ui->file_path->text()),
                    QString::fromUtf8("知道了"));
            return;
        }
        //
        // 保存设置。
        QSettings settings;
        settings.setValue(SETTING_DATASOURCE_FILE_PATH,ui->file_path->text());
        settings.setValue(SETTING_DATASOURCE_FILE_PERIOD,period);
        //
        // 界面。
        m_file_running = true;
        m_file_pause = false;
        ui->file_startStop->setText(QString::fromUtf8("停止"));
        ui->file_pathGroup->setEnabled(!m_file_running);
        ui->file_optionGroup->setEnabled(!m_file_running);
        ui->file_pauseContinue->setEnabled(m_file_running);
        ui->file_pauseContinue->setText(QString::fromUtf8("暂停"));
        ui->serial->setEnabled(!m_file_running);
    }
}

void DialogDataSource::on_file_pauseContinue_clicked()
{
    if(m_file_pause)
    {
        m_file_pause = false;
        m_textFileParser.continue_();
        ui->file_pauseContinue->setText(QString::fromUtf8("暂停"));
    }
    else
    {
        m_file_pause = true;
        m_textFileParser.pause();
        ui->file_pauseContinue->setText(QString::fromUtf8("继续"));
    }
}

void DialogDataSource::on_file_stop()
{
    //
    // 界面。
    m_file_running = false;
    m_file_pause = false;
    ui->file_startStop->setText(QString::fromUtf8("开始"));
    ui->file_pathGroup->setEnabled(!m_file_running);
    ui->file_optionGroup->setEnabled(!m_file_running);
    ui->file_pauseContinue->setEnabled(m_file_running);
    ui->file_pauseContinue->setText(QString::fromUtf8("暂停"));
    ui->serial->setEnabled(!m_file_running);
}

void DialogDataSource::serial_send(const QByteArray &bytes)
{
    if(m_serial != 0)
        emit m_serial->write(bytes);
    if(m_usb != 0)
        emit m_usb->writeData(0, bytes);
}

void DialogDataSource::on_cbPrintTransmitPacket_stateChanged(int arg1)
{
    m_serialBinaryParser.needPrintTransmitPacket(arg1);
}

void DialogDataSource::on_cbPrintReceivePacket_stateChanged(int arg1)
{
    m_serialBinaryParser.needPrintReceivePacket(arg1);
}

void DialogDataSource::on_serial_refresh_clicked()
{
    ui->serial_port->clear();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach(const QextPortInfo& info, ports){
        ui->serial_port->addItem(info.portName);
    }
    ui->serial_port->setCurrentIndex(0);
}

void DialogDataSource::on_usb_openClose_clicked()
{
    if(!m_usb_opening)
    {
        m_usb_opening = true;
        //
        // 打开USB。
        QString path = ui->usb_path->text();
        m_usb = new QUsbHid();
        m_usb->setPath(path);
        if(!m_usb->open(QIODevice::ReadWrite))
        {
            delete m_usb;
            m_usb = 0;
            //
            QMessageBox::warning(this,QString::fromUtf8("错误"),
                                 QString::fromUtf8("打开USB%1失败！").arg(path));
            return;
        }
        m_usb->flush();
        connect(m_usb, SIGNAL(readyRead()), this, SLOT(every30ms()));
        //
        // 界面。
        ui->usb_openClose->setText(QString::fromUtf8("关闭USB"));
        ui->serial->setEnabled(!m_usb_opening);
        ui->file->setEnabled(!m_usb_opening);
    }
    else
    {
        m_usb_opening = false;
        //
        m_usb->close();
        delete m_usb;
        m_usb = 0;
        //
        // 界面。
        ui->usb_openClose->setText(QString::fromUtf8("打开USB"));
        ui->serial->setEnabled(!m_usb_opening);
        ui->file->setEnabled(!m_usb_opening);
    }
}

void DialogDataSource::on_usb_refresh_clicked()
{
    ui->usbHid->clear();
    QList<QUsbHidInfo> devs = QUsbHid::enumDevices(0x250,0x250);
    foreach(const QUsbHidInfo& info, devs){
        ui->usbHid->addItem(info.friendName, info.path);
    }
    ui->usbHid->setCurrentIndex(0);
}

void DialogDataSource::on_usbHid_currentIndexChanged(int index)
{
    ui->usb_path->setText( ui->usbHid->itemData(index).toString() );
}
