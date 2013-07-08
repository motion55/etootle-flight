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

#include "gltrace.h"

// size为轨迹最大点数。
GlTrace::GlTrace(unsigned int lineAmountMax)
{
    m_array = 0;
    m_arraySize = 0;
    m_arrayIndexHead = 0;
    m_arrayIndexTail = 0;
    m_lineWidth = 1;
    //
    reset(lineAmountMax);
}

// 从新设置最大点数。
// 成功返回true，否则false。
bool GlTrace::reset(unsigned int lineAmountMax)
{
    if(lineAmountMax == 0)
        return false;
    if(m_array != 0)
        delete m_array;
    //
    // 两点一线+1，区分头尾索引+1，首尾相同+1。
    m_arraySize = lineAmountMax + 3;
    m_array = new float[m_arraySize * 3]; // 3 => xyz
    //
    if(m_array == 0)
    {
        m_arraySize = 0;
        return false;
    }
    //
    m_arrayIndexHead = 0;
    m_arrayIndexTail = 0;
    return true;
}

void GlTrace::push(const float p[])
{
    if(m_array == 0 || m_arraySize == 0)
        return;
    //
    copyPoint(p,&m_array[m_arrayIndexHead * 3]);
    //
    m_arrayIndexHead ++; // 推进head。
    if(m_arrayIndexHead < m_arraySize)
    {
        // head在中间的情况。
        // 如果head、tail相等，就推进tail，丢弃最后的数据。
        if(m_arrayIndexHead == m_arrayIndexTail)
        {
            m_arrayIndexTail ++;
            //
            // 还要判断tail是否移到最后一个单元，如果是就也要丢弃。
            if(m_arrayIndexTail == m_arraySize-1)
                m_arrayIndexTail = 0;
        }
    }
    else
    {
        // 否则，就是刚刚把数据放在最后一个单元的情况了。
        // 先把数据复制一份在第一个单元。
        // 再把head设为1。
        // 最后判断tail需不需要改变。
        copyPoint(p,m_array);
        m_arrayIndexHead = 1;
        if(m_arrayIndexTail == 0 || m_arrayIndexTail == 1)
            m_arrayIndexTail = 2;
    }
}

void GlTrace::draw()
{
    m_material.useMaterial();
    glLineWidth(m_lineWidth);
    //
    // 使用顶点数组。
    glEnableClientState(GL_VERTEX_ARRAY);
    {
        if(m_arrayIndexTail <= m_arrayIndexHead)
        {
            // 只有一段的情况。
            //
            int len = m_arrayIndexHead - m_arrayIndexTail;
            if(len < 2)
                return;
            glVertexPointer(3,GL_FLOAT,0,&(m_array[m_arrayIndexTail * 3]));
            glDrawArrays(GL_LINE_STRIP,0,len);
        }
        else
        {
            // 分两段的情况。
            //
            // 前半段。
            if(m_arrayIndexHead > 2)
            {
                glVertexPointer(3,GL_FLOAT,0,m_array);
                glDrawArrays(GL_LINE_STRIP,0,m_arrayIndexHead);
            }
            //后半段。
            int len = m_arraySize - m_arrayIndexTail;
            if(len > 2)
            {
                glVertexPointer(3,GL_FLOAT,0,&(m_array[m_arrayIndexTail * 3]));
                glDrawArrays(GL_LINE_STRIP,0,len);
            }
        }
    }glDisableClientState(GL_VERTEX_ARRAY);
}

void GlTrace::copyPoint(const float from[], float to[])
{
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
}
