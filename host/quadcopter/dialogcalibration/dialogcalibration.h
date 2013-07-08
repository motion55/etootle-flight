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

#ifndef DIALOGCALIBRATION_H
#define DIALOGCALIBRATION_H

#include <stdint.h>
#include <QDialog>
#include <QTimer>
#include <QLineEdit>
#include "../protocol.h"

namespace Ui {
class DialogCalibration;
}

class DialogCalibration : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCalibration(QWidget *parent = 0);
    ~DialogCalibration();

signals:
    void getXyz(uint8_t type);
    void makeGyrOffset(QVector<float> offset);
    void makeAccParam(QVector<float> param);
    void makeMagParam(QVector<float> param);

public slots:
    void onGetXyz(uint8_t type,float x,float y, float z);

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_bt_input_clicked();
    void on_bt_clear_clicked();
    void on_bt_calculate_clicked();
    void on_bt_refleshDisplay_clicked();
    void on_displayEllipsoid_stateChanged(int arg1);
    void on_btCheckValue_clicked();
    void on_btCheckLength_clicked();
    void on_autoGetData_stateChanged(int arg1);
    void onAutoGetDataTimer();
    void on_dataType_currentIndexChanged(int index);
    void on_btMakeGyrOffset_clicked();
    void on_btMakeAccParam_clicked();
    void on_btMakeMagParam_clicked();

private:
    unsigned int getUintFromLineEdit(QLineEdit *from, unsigned int default_);
    float getFloatFromLineEdit(QLineEdit *from, float default_);
    void setupDataOption();

private:
    Ui::DialogCalibration *ui;
    int m_sampleCountdown;
    bool m_isSampling;
    float m_preOffset[3];
    float m_xyzDisplay[3];
    float m_resultOffset_noPre[3];
    float m_resultScale[3];
    float m_radius;
    QTimer m_timerAutoGetData;
    int m_dataType;
};

#endif // DIALOGCALIBRATION_H
