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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtNetwork>

MainWindow * MainWindow::m_this = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dialogControl(this),
    m_dialogParameter(this),
    m_dialogCalibration(this),
    m_dialogScene(this),
    m_dialogDataSource(this),
    m_dialogStatus(this),
    m_dialogBootloader(this)
{
    ui->setupUi(this);
    m_this = this;
    printInitInfo();
    //
    // 标题加上编译时间，方便区别版本。
    QString date = QLocale(QLocale::C).toDate(QString(__DATE__).replace("  "," ")
                                              ,"MMM d yyyy").toString("yyyy-MM-dd");
    QString time = QString(__TIME__).left(5);
    setWindowTitle(QString("%1 %2 %3").arg(windowTitle()).arg(date).arg(time));
    //
    BinaryParser & binaryParser = m_dialogDataSource.getBinaryParser();
    TextFileParser & textFileParser = m_dialogDataSource.getTextParser();
    //
    // 文本解析器。
    connect(&textFileParser,SIGNAL(displayTranslation(float,float,float)),
            &m_dialogScene,SLOT(displayPosition(float,float,float)));
    //
    // 场景面板与串口二进制解析器。
    connect(&binaryParser,SIGNAL(displayAttitudeQuaternion(Quaternion)),
            &m_dialogScene,SLOT(displayAttitude(Quaternion)));
    connect(&binaryParser,SIGNAL(displayTranslation(float,float,float)),
            &m_dialogScene,SLOT(displayPosition(float,float,float)));
    connect(&m_dialogScene,SIGNAL(getAttitude()),
            &binaryParser,SLOT(cmd_getAttitudeQuaternion()));
    connect(&m_dialogScene,SIGNAL(getPosition()),
            &binaryParser,SLOT(cmd_getTranslation()));
    //
    // 控制面板与串口二进制解析器。
    connect(&m_dialogControl,SIGNAL(setMode(uint8_t)),
            &binaryParser,SLOT(control_setMode(uint8_t)));
    connect(&m_dialogControl,SIGNAL(sendHeartbeat()),
            &binaryParser,SLOT(control_sendHeartbeat()));
    connect(&m_dialogControl,SIGNAL(lockThrottle_setThrottle(QVector<float>)),
            &binaryParser,SLOT(control_lockThrottle_setThrottle(QVector<float>)));
    connect(&m_dialogControl,SIGNAL(lockAttitude(QByteArray)),
            &binaryParser,SLOT(control_lockAttitude(QByteArray)));
    connect(&binaryParser,SIGNAL(onControlLockAttitude(QByteArray)),
            &m_dialogControl,SLOT(onLockAttitude(QByteArray)));
    connect(&m_dialogControl,SIGNAL(on_tcp_data(QByteArray)),
            &binaryParser, SLOT(onReceivedData(QByteArray)));
    connect(&binaryParser,SIGNAL(onControlRawData(QByteArray)),
            &m_dialogControl, SLOT(onContralRawData(QByteArray)));

    //
    // 参数面板与二进制解析器。
    connect(&m_dialogParameter,SIGNAL(parameter(QByteArray)),
            &binaryParser,SLOT(cmd_parameter(QByteArray)));
    connect(&binaryParser,SIGNAL(onParameter(QByteArray)),
            &m_dialogParameter,SLOT(onParameter(QByteArray)));
    //
    // 校正面板与二进制解析器。
    connect(&binaryParser,SIGNAL(onReturnVector(uint8_t,float,float,float)),
            &m_dialogCalibration,SLOT(onGetXyz(uint8_t,float,float,float)));
    connect(&m_dialogCalibration,SIGNAL(getXyz(uint8_t)),
            &binaryParser,SLOT(cmd_getVector(uint8_t)));
    //
    // 状态对话框与串口二进制解析器。
    connect(&binaryParser,SIGNAL(onReturnStatus(uint8_t,QVector<uint8_t>)),
            &m_dialogStatus,SLOT(onReturnStatus(uint8_t,QVector<uint8_t>)));
    connect(&m_dialogStatus,SIGNAL(getStatus(QVector<uint8_t>)),
            &binaryParser,SLOT(cmd_getStatus(QVector<uint8_t>)));
    //
    // 主对话框与串口二进制解析器。
    connect(&binaryParser,SIGNAL(onReturnMessage(QString)),
            this,SLOT(onReturnMessage(QString)));
    //
    // 更新程序对话框与串口二进制解析器。
    connect(&m_dialogBootloader,SIGNAL(cmd(uint8_t,QByteArray)),
            &binaryParser,SLOT(cmd_bootloadCmd(uint8_t,QByteArray)));
    connect(&binaryParser,SIGNAL(onBootloaderStatus(uint8_t,QByteArray)),
            &m_dialogBootloader,SLOT(onStatus(uint8_t,QByteArray)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printMessage(const QString &msg)
{
    static int index = 0;
    if(! m_this)
        return;
    //
    m_this->ui->tbMessage->moveCursor(QTextCursor::End);
    m_this->ui->tbMessage->insertHtml(QString("%1.%2<br>").arg(index++).arg(msg));
    m_this->ui->tbMessage->moveCursor(QTextCursor::End);
}

void MainWindow::onReturnMessage(const QString &msg)
{
    printMessage(QString("msg : %1").arg(msg));
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    // 默认布局大小不会自动变，要手动设置。
    QSize s = event->size();
    QRect r(0,0,s.width(),s.height());
    ui->horizontalLayoutWidget->setGeometry(r);
}

void MainWindow::on_btDialogDataSource_clicked()
{
    m_dialogDataSource.showNormal();
    m_dialogDataSource.raise();
}

void MainWindow::on_btDialogControl_clicked()
{
    m_dialogControl.showNormal();
    m_dialogControl.raise();
}

void MainWindow::on_btDialogParameter_clicked()
{
    m_dialogParameter.showNormal();
    m_dialogParameter.raise();
}

void MainWindow::on_btDialogCalibration_clicked()
{
    m_dialogCalibration.showNormal();
    m_dialogCalibration.raise();
}

void MainWindow::on_btDialogScene_clicked()
{
    m_dialogScene.showNormal();
    m_dialogScene.raise();
}

void MainWindow::on_btDialogStatus_clicked()
{
    m_dialogStatus.showNormal();
    m_dialogStatus.raise();
}

void MainWindow::on_btDialogBootloader_clicked()
{
    m_dialogBootloader.showNormal();
    m_dialogBootloader.raise();
}

void MainWindow::on_btExecute_clicked()
{
    QString cmd = ui->leCmd->text().trimmed();
    ui->leCmd->clear();
    ui->leCmd->setFocus();
    //
    if(! cmd.isEmpty())
        printMessage(QString::fromUtf8("cmd : 没有这个命令(%1)").arg(cmd));
}

void MainWindow::on_btClearMessage_clicked()
{
    ui->tbMessage->clear();
}

void MainWindow::printInitInfo()
{
    printMessage(QString::fromUtf8
                 ("欢迎使用四轴上位机。\n"
                  "正在获取最新信息..."));
    //
    QUrl url("http://www.etootle.com/product/4xaircraft/4x_version_info.html");
    //QUrl url("4x_version_info.html");
    //
    m_reply = m_network.get(QNetworkRequest(url));
    connect(m_reply,SIGNAL(finished()),this,SLOT(onReplyFinish()));
}

void MainWindow::onReplyFinish()
{
    if(m_reply->error() != QNetworkReply::NoError)
    {
        printMessage(QString::fromUtf8("获取信息失败:") + m_reply->errorString());
        return;
    }
    //
    QByteArray data = m_reply->readAll();
    printMessage(QString::fromUtf8(data));
    //
    m_reply->deleteLater();
}
