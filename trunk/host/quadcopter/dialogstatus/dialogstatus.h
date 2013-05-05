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

#ifndef DIALOGSTATUS_H
#define DIALOGSTATUS_H

#include <stdint.h>
#include <QtGui>
#include <QVector>
#include <QTimer>

namespace Ui {
class DialogStatus;
}

class DialogStatus : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogStatus(QWidget *parent = 0);
    ~DialogStatus();

signals :
    void getStatus(const QVector<uint8_t> &which);

public slots:
    void onReturnStatus(uint8_t which,const QVector<uint8_t> &value);

private slots:
    void on_btGet_clicked();
    void onTimer(void);
    void on_cbAutoGet_stateChanged(int arg1);

private:
    unsigned int getUintFromLineEdit(QLineEdit *from, unsigned int default_);

private:
    Ui::DialogStatus *ui;
    QTimer m_timerGet;
};

#endif // DIALOGSTATUS_H
