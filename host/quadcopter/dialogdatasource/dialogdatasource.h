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

#ifndef DIALOGDATASOURCE_H
#define DIALOGDATASOURCE_H

#include <QDialog>
#include "textfileparser.h"
#include <QLineEdit>
#include <QTimer>
#include "qextserialport/qextserialport.h"
#include "binaryparser.h"

namespace Ui {
class DialogDataSource;
}

class DialogDataSource : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogDataSource(QWidget *parent = 0);
    ~DialogDataSource();
    BinaryParser & getBinaryParser();
    TextFileParser & getTextParser();

private slots:
    void every30ms();
    void on_serial_openClose_clicked();
    void on_serial_checkCrc_stateChanged(int arg1);
    void on_file_selectFile_clicked();
    void on_file_startStop_clicked();
    void on_file_pauseContinue_clicked();
    void on_file_stop();
    void serial_send(const QByteArray &bytes);

    void on_cbPrintTransmitPacket_stateChanged(int arg1);

    void on_cbPrintReceivePacket_stateChanged(int arg1);

private:
    void initFileSetting();
    void initSerialSetting();
    void connectSignalSlot();
    void dataSorce_getSerialSetting(PortSettings &setting);
    unsigned int getUintFromLineEdit(QLineEdit *from, unsigned int default_);

private:
    const static char * SETTING_DATASOURCE_SERIAL_PORT;
    const static char * SETTING_DATASOURCE_SERIAL_BOUNDRATE;
    const static char * SETTING_DATASOURCE_SERIAL_DATABITS;
    const static char * SETTING_DATASOURCE_SERIAL_PARITY;
    const static char * SETTING_DATASOURCE_SERIAL_STOPBITS;
    const static char * SETTING_DATASOURCE_FILE_PATH;
    const static char * SETTING_DATASOURCE_FILE_PERIOD;

private:
    Ui::DialogDataSource *ui;
    bool m_file_running; // 运行状态，是否正在读文件。
    bool m_file_pause;   // 运行状态，读文件是否被暂停。
    TextFileParser m_textFileParser; // 文本文件解析器。
    QextSerialPort * m_serial;
    bool m_serial_opening;
    BinaryParser m_serialBinaryParser;
    QTimer m_serial_timer;
};

#endif // DIALOGDATASOURCE_H
