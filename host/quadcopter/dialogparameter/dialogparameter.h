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

#ifndef DIALOGPARAMETER_H
#define DIALOGPARAMETER_H

#include <QtGui>
#include <stdint.h>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QMap>
#include <QPair>

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

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_btGetParameter_clicked();
    void on_btSetParameter_clicked();

    void on_btClear_clicked();

    void on_btWriteFlash_clicked();

    void on_btResetFlash_clicked();

private:
    float getFloat(QLineEdit *from, float default_=0.0f);
    void setupAreas(void);
    void getParameter(int pos);
    void setParameter(int pos,float value);

private:
    Ui::DialogParameter *ui;
    QStandardItemModel m_tableViewModel;

private:
    class Area
    {
    public:
        void addToModel(QStandardItemModel & model);
        int getRightIndex(void) { return m_leftIndex + m_horizontalList.size();}
        int getBottomIndex(void) { return m_topIndex + m_verticalList.size();}
        int hit(int row,int col);

    public:
        int m_leftIndex,m_topIndex;
        int m_posBase;
        QString m_groupName;
        QVector<QString> m_verticalList;
        QVector<QString> m_horizontalList;
    };
    QVector<Area> m_areas;
    QMap<int,QPair<int,int> > m_posToRowCol;
};

#endif // DIALOGPARAMETER_H
