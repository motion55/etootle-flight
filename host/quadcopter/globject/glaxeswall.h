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

#ifndef GLAXESWALL_H
#define GLAXESWALL_H

#include "globject.h"
#include "glgrid.h"
#include "gltext.h"

class GlAxesWall
{
public:
    GlAxesWall();
    void setCenter(float const center[3]);
    void setCenter(float x, float y, float z);
    void setSize(float half);
    virtual void draw();
private:
    void updateGraduation();
private:
    float m_center[3]; // 中心。
    float m_sizeHalf; // 长方体边长一半。
    GlGrid m_xy;
    GlGrid m_yz;
    GlGrid m_zx;
    GlText m_graduation[6]; // x中间，x正端，y中间，y正端，z中间，z正端。
};

#endif // GLAXESWALL_H
