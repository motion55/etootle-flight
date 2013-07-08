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

#include "glaxeswall.h"

GlAxesWall::GlAxesWall()
{
    m_sizeHalf = 1;
    for(int i=0;i<3;i++)
    {
        m_center[i] = 0;
    }
    updateGraduation();
    //
    m_xy.make(11,11);
    m_yz.make(11,11);
    m_zx.make(11,11);
    float v_xy[3] = {0,0,-1};
    float v_yz[3] = {-1,0,0};
    float v_zx[3] = {0,-1,0};
    float n_xy[3] = {0,0,1};
    float n_yz[3] = {1,0,0};
    float n_zx[3] = {0,1,0};
    for(int i=0;i<11;i++)
    {
        v_xy[0] = v_yz[1] = v_zx[2] = (i - 5) / 5.0;
        for(int j=0;j<11;j++)
        {
            v_xy[1] = v_yz[2] = v_zx[0] = (j - 5) / 5.0;
            //
            m_xy.setVertex(i,j,v_xy);
            m_xy.setNormal(i,j,n_xy);
            m_yz.setVertex(i,j,v_yz);
            m_yz.setNormal(i,j,n_yz);
            m_zx.setVertex(i,j,v_zx);
            m_zx.setNormal(i,j,n_zx);
        }
    }
    m_xy.getMaterialLine()->loadColor(0,0,0.5);
    m_xy.getMaterialSurface()->loadColor(0.75,0.75,0);
    m_xy.needDrawLine(true);
    m_xy.needDrawSurface(true);
    m_yz.getMaterialLine()->loadColor(0.5,0,0);
    m_yz.getMaterialSurface()->loadColor(0,0.75,0.75);
    m_yz.needDrawLine(true);
    m_yz.needDrawSurface(true);
    m_zx.getMaterialLine()->loadColor(0,0.5,0);
    m_zx.getMaterialSurface()->loadColor(0.75,0,0.75);
    m_zx.needDrawLine(true);
    m_zx.needDrawSurface(true);
}

void GlAxesWall::setCenter(const float center[3])
{
    for(int i=0;i<3;i++)
        m_center[i] = center[i];
    updateGraduation();
}

void GlAxesWall::setCenter(float x, float y, float z)
{
    m_center[0] = x;
    m_center[1] = y;
    m_center[2] = z;
    updateGraduation();
}

void GlAxesWall::setSize(float half)
{
    if(half < 0)
        return;
    m_sizeHalf = half;
    updateGraduation();
}

void GlAxesWall::draw()
{
    glPushMatrix();
    //
    glTranslatef(m_center[0],m_center[1],m_center[2]);
    glScalef(m_sizeHalf,m_sizeHalf,m_sizeHalf);
    //
    m_xy.draw();
    m_yz.draw();
    m_zx.draw();
    //
    for(int i=0;i<6;i++)
        m_graduation[i].draw();
    //
    glPopMatrix();
}

void GlAxesWall::updateGraduation()
{
    QString str;
    const float NE = -0.9;
    const float CE = 0;
    m_graduation[0].setPosition(CE,NE,NE);
    m_graduation[0].setText(str.sprintf("%.2f",m_center[0]));
    m_graduation[2].setPosition(NE,CE,NE);
    m_graduation[2].setText(str.sprintf("%.2f",m_center[1]));
    m_graduation[4].setPosition(NE,NE,CE);
    m_graduation[4].setText(str.sprintf("%.2f",m_center[2]));
    //
    m_graduation[1].setPosition(1,NE,NE);
    m_graduation[1].setText(str.sprintf("%.2f",m_center[0] + m_sizeHalf));
    m_graduation[3].setPosition(NE,1,NE);
    m_graduation[3].setText(str.sprintf("%.2f",m_center[1] + m_sizeHalf));
    m_graduation[5].setPosition(NE,NE,1);
    m_graduation[5].setText(str.sprintf("%.2f",m_center[2] + m_sizeHalf));
}

