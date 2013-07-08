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

#ifndef DIALOGSCENE_H
#define DIALOGSCENE_H

#include <QDialog>
#include "../quaternion.h"
#include <QTimer>
#include <QLineEdit>
#include <QResizeEvent>

namespace Ui {
class DialogScene;
}

class DialogScene : public QDialog
{
    Q_OBJECT

public:
    explicit DialogScene(QWidget *parent = 0);
    ~DialogScene();

signals:
    void getAttitude();
    void getPosition();

public slots:
    void displayAttitude(const Quaternion & atti);
    void displayPosition(float x,float y,float z);

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_autoGetAttitude_stateChanged(int arg1);
    void on_autoGetPosition_stateChanged(int arg1);
    void on_resetView_clicked();
    void on_fullscreen_stateChanged(int arg1);
    void on_displayWall_stateChanged(int arg1);
    void on_displayModel_stateChanged(int arg1);
    void on_displayModelAxes_stateChanged(int arg1);
    void on_displayWorldAxes_stateChanged(int arg1);

private:
    unsigned int getUintFromLineEdit(QLineEdit *from, unsigned int default_);

private:
    Ui::DialogScene *ui;
    QTimer m_timerAutoGetAttitude;
    QTimer m_timerAutoGetPosition;
};

#endif // DIALOGSCENE_H
