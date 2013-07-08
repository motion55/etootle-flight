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
