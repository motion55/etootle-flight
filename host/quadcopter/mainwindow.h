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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTimer>
#include <QtNetwork>
#include "dialogparameter/dialogparameter.h"
#include "dialogcontrol/dialogcontrol.h"
#include "dialogcalibration/dialogcalibration.h"
#include "dialogscene/dialogscene.h"
#include "dialogdatasource/dialogdatasource.h"
#include "dialogstatus/dialogstatus.h"
#include "dialogbootloader/dialogbootloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static void printMessage(const QString & msg);

public slots:
    void onReturnMessage(const QString & msg);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_btDialogDataSource_clicked();
    void on_btDialogControl_clicked();
    void on_btDialogParameter_clicked();
    void on_btDialogCalibration_clicked();
    void on_btDialogScene_clicked();
    void on_btDialogStatus_clicked();
    void on_btDialogBootloader_clicked();
    void on_btExecute_clicked();
    void on_btClearMessage_clicked();
    void onReplyFinish(void);

private:
    void printInitInfo(void);

private:
    Ui::MainWindow *ui;
    DialogControl m_dialogControl;
    DialogParameter m_dialogParameter;
    DialogCalibration m_dialogCalibration;
    DialogScene m_dialogScene;
    DialogDataSource m_dialogDataSource;
    DialogStatus m_dialogStatus;
    DialogBootloader m_dialogBootloader;
    static MainWindow * m_this;
    QNetworkAccessManager m_network;
    QNetworkReply *m_reply;
};

#endif // MAINWINDOW_H
