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

#include "filereader.h"
#include <QFile>
#include "mymath.h"

FileReader::FileReader(QObject *parent) :
    QObject(parent)
{
}

int FileReader::readFile(QString path)
{
    QFile file(path);
    if(! file.open(QFile::ReadOnly))
    {
        m_fileContent.clear();
        return -1;
    }
    //
    m_fileContent = file.readAll();
    //
    // 补足长度，使长度成整数块。
    int left = m_fileContent.length() % BYTE_PER_BLOCK;
    if(left != 0)
    {
        left = 4-left;
        for(int i=0;i<left;i++)
            m_fileContent.append((char)0);
    }
    //
    m_blocksAmount = m_fileContent.length() / 4;
    m_currentRegion.index = 0;
    m_currentRegion.length = m_blocksAmount;
    m_regionsNotWrite.clear();
    //
    return 0;
}

int FileReader::getNextBlocks(QByteArray *blocks, int maxSize)
{
    // 保证m_currentRegion有效。
    if(m_currentRegion.length <= 0)
    {
        if(m_regionsNotWrite.isEmpty())
            return -1;
        m_currentRegion = m_regionsNotWrite.dequeue();
    }
    //
    // 求实际的长度。
    int len = (maxSize-2)/BYTE_PER_BLOCK;
    if(len > m_currentRegion.length)
        len = m_currentRegion.length;
    //
    // 复制数据，推进指针。
    uint16_t t = m_currentRegion.index;
    blocks->clear();
    blocks->append(((uint8_t *)&t)[0]);
    blocks->append(((uint8_t *)&t)[1]);
    blocks->append(m_fileContent.mid(m_currentRegion.index*BYTE_PER_BLOCK,len*BYTE_PER_BLOCK));
    m_currentRegion.index  += len;
    m_currentRegion.length -= len;
    //
    return 0;
}

int FileReader::getLengthCrc(uint32_t *length_block, uint32_t *crc)
{
    if(m_fileContent.size() <= 0)
        return -1;
    //
    *length_block = m_blocksAmount;
    *crc = MyMath::crc16(0,m_fileContent.constData(),m_fileContent.size());
    //
    return 0;
}

int FileReader::addRegion(int index, int length)
{
    if(index < 0 || length <= 0 || index+length > m_blocksAmount)
        return -1;
    //
    Region r;
    r.index = index;
    r.length = length;
    //
    m_regionsNotWrite.enqueue(r);
    //
    return 0;
}

int FileReader::getBlocksAmount()
{
    return m_blocksAmount;
}

int FileReader::isNoMoreblock()
{
    return ((m_currentRegion.length == 0) && (m_regionsNotWrite.length() == 0));
}
