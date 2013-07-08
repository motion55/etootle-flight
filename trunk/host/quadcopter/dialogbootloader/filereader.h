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

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QByteArray>
#include <QQueue>

class FileReader : public QObject
{
    Q_OBJECT
public:
    explicit FileReader(QObject *parent = 0);
    int readFile(QString path);
    int getNextBlocks(QByteArray *blocks,int maxSize);
    int getLengthCrc(uint32_t *length_block,uint32_t *crc);
    int addRegion(int index,int length);
    int getBlocksAmount(void);
    int isNoMoreblock(void);
    
signals:
    
public slots:

private:
    class Region
    {
    public:
        Region() { index = length = 0; }
        //
        int index;
        int length;
    };

private:
    const static int BYTE_PER_BLOCK = 4;

private:
    QByteArray m_fileContent;
    Region m_currentRegion;
    QQueue<Region> m_regionsNotWrite;
    int m_blocksAmount;
};

#endif // FILEREADER_H
