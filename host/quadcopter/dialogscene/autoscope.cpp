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

#include "autoscope.h"

AutoScope::AutoScope(const GlView &v, QObject *parent) :
    QObject(parent),
    m_view(v)
{
    for(int i=0;i<3;i++)
        m_lastLeft[i] = false;
}

void AutoScope::updatePosition(const float pos[])
{
    float center[3];
    m_view.getViewPoint(center);
    float sizeHalf = m_view.getHorizon();
    //
    for(int i=0;i<3;i++)
    {
        if(pos[i] < center[i]-sizeHalf)
        {
            // 左边越界。
            if(m_lastLeft[i])
            {
                // 现在是左边，上次也是左边。
                center[i] = pos[i] + sizeHalf/2;
            }
            else
            {
                // 现在是左边，上次是右边。
                while(pos[i] < center[i]-sizeHalf)
                {
                    sizeHalf *= 1.5;
                }
            }
            m_lastLeft[i] = true;
        }
        else if(pos[i] > center[i]+sizeHalf)
        {
            // 右边越界。
            if(m_lastLeft[i])
            {
                // 现在是右边，上次是左边。
                while(pos[i] > center[i]+sizeHalf)
                {
                    sizeHalf *= 1.5;
                }
            }
            else
            {
                // 现在是右边，上次也是右边。
                center[i] = pos[i] - sizeHalf/2;
            }
            m_lastLeft[i] = false;
        }
    }
    //
    emit setView(center[0],center[1],center[2],sizeHalf);
}
