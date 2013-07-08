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

#ifndef DIALOGPARAMETER_H
#define DIALOGPARAMETER_H

#include <QtGui>
#include <stdint.h>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QMap>
#include <QPair>
#include "quaternion.h"

namespace Ui {
class DialogParameter;
}

class DialogParameter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogParameter(QWidget *parent = 0);
    ~DialogParameter();

signals:
    void parameter(const QByteArray & param);

public slots:
    void onParameter(const QByteArray &param);
    void onMakeGyrOffset(QVector<float> offset);
    void onMakeAccParam(QVector<float> param);
    void onMakeMagParam(QVector<float> param);
    void onMakeHorizontalAttitude(Quaternion q);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_btGetParameter_clicked();
    void on_btSetParameter_clicked();
    void on_btClear_clicked();
    void on_btWriteFlash_clicked();
    void on_btResetFlash_clicked();
    void on_btShowHideExtend_clicked();
    void on_btMixHorizontalAttitude_clicked();

private:
    void setupAreas(void);
    QPair<int,int> getSelections(void);
    void changeRowStatus(int row,QString status);
    void setParameter(uint8_t pos,const QVector<int32_t> &value);

private:
    Ui::DialogParameter *ui;
    QStandardItemModel m_tableViewModel;
    QVector<QString> m_rows;

private:
    enum
    {
        COLUMN_INDEX_NAME  = 0,
        COLUMN_INDEX_GET    = 1,
        COLUMN_INDEX_SET    = 2,
        COLUMN_INDEX_STATUS = 3
    };
};

#endif // DIALOGPARAMETER_H
