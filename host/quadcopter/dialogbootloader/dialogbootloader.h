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

#ifndef DIALOGBOOTLOADER_H
#define DIALOGBOOTLOADER_H

#include <QtGui>
#include <QByteArray>
#include <QTimer>
#include "filereader.h"

namespace Ui {
class DialogBootloader;
}

class DialogBootloader : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogBootloader(QWidget *parent = 0);
    ~DialogBootloader();
    
signals:
    void cmd(uint8_t cmd,QByteArray param);

public slots:
    void onStatus(uint8_t status,const QByteArray param);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_btSelectFile_clicked();
    void on_btStartRefreshApp_clicked();
    void onTimer(void);

private:
    class Status
    {
    public:
        const static int INIT                   = 1;
        const static int ENTERING_BOOTLOADER    = 2;
        const static int BOOTLOADER_READY       = 3;
        const static int READY_TO_REFRESH_APP   = 4;
        const static int WRITING_APP            = 5;
        //const static int WAITING_APP_FINISH     = 6;
        const static int REFRESH_APP_SUCCEED    = 7;
        const static int REFRESH_APP_FAILED     = 8;
        const static int RUNNING_APP            = 9;
    };

private:
    int sendLengthCrc();
    int onWriting(const QByteArray &param);
    int onRemainRegion(const QByteArray &raw);
    void refreshComplete(bool isSucceed);
    void writeSomeBlock(void);

private:
    Ui::DialogBootloader *ui;
    QTimer m_timer;
    int m_status;
    FileReader m_fileReader;
};

#endif // DIALOGBOOTLOADER_H
