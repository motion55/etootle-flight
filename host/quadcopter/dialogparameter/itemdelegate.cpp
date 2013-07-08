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

#include "itemdelegate.h"
#include <QtGui>

ItemDelegate::ItemDelegate()
{
}

QWidget *ItemDelegate::ReadOnlyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 返回0以取消显示编辑控件，即把Item设为了只读。
    return 0;
}

QWidget *ItemDelegate::FloatValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit * le = new QLineEdit(parent);
    return le;
}

void ItemDelegate::FloatValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit * le = static_cast<QLineEdit *>(editor);
    le->setText(index.data(Qt::EditRole).toString());
}

void ItemDelegate::FloatValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit * le = static_cast<QLineEdit *>(editor);
    bool ok = false;
    float value = le->text().toFloat(&ok);
    if(ok)
    {
        model->setData(index,QVariant(QString("%1").arg(value)));
    }
}

void ItemDelegate::FloatValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
