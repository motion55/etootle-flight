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
