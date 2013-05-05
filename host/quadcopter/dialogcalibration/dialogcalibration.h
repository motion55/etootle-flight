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
public slots:
    void onGetXyz(uint8_t type,float x,float y, float z);
protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
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

private:
    unsigned int getUintFromLineEdit(QLineEdit *from, unsigned int default_);
    float getFloatFromLineEdit(QLineEdit *from, float default_);
    void setupDataOption();
private:
    Ui::DialogCalibration *ui;
    int m_sampleCountdown;
    bool m_idSampling;
    float m_preOffset[3];
    float m_xyzDisplay[3];
    float m_resultOffset_noPre[3];
    float m_resultScale[3];
    float m_radius;
    QTimer m_timerAutoGetData;
    int m_dataType;
};

#endif // DIALOGCALIBRATION_H
