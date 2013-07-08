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

#ifndef GLTRACE_H
#define GLTRACE_H

#include "globject.h"
#include "material.h"

class GlTrace : public GlObject
{
public:
    explicit GlTrace(unsigned int lineAmountMax = 0);
    bool reset(unsigned int lineAmountMax);
    void push(const float p[3]);
    void setLineWidth(float w) {m_lineWidth = w > 0 ? w : 1;}
    Material * getMaterial() {return &m_material;}
    virtual void draw();
private:
    void copyPoint(const float from[3],float to[3]);
private:
    Material m_material;
    float * m_array; // 点(xyz)数组。
    int m_arraySize; // 点数组大小/3。
    int m_arrayIndexHead;
    int m_arrayIndexTail;
    float m_lineWidth;
};

/* 1. m_array以3个float为单元，大小为m_arraySize个单元。
 * 2. m_array为循环队列。
 * 3. m_arrayIndexHead为下一个单元，数据无效。
 * 4. m_arrayIndexTail为最后一个单元，数据有效。
 * 5. m_arrayIndexHead等于m_arrayIndexTail时队列空。
 * 6. m_array第1个单元与最后一个单元数据相同，方便OpenGL数组绘制。
 * 7. 如果队列非空，m_arrayIndexHead不会为0，m_arrayIndexTail不会为(m_arraySize-1)。
 *
 * m_arraySize等于8时
 * 0 1 2 3 4 5 6 7
 * ■ ■ ■ □ □ □ ■ ■
 * ↑     ↑     ↑ ↑
 * |  head  tail |
 * +---同一个点---+
 */

#endif // GLTRACE_H
