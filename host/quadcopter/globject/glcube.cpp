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

#include "glcube.h"
#include <GL/gl.h>

GlCube::GlCube()
{
    genericInit();
    load(1.0f,1.0f,1.0f);
}

GlCube::GlCube(float x, float y, float z)
{
    genericInit();
    load(x, y, z);
}

void GlCube::genericInit(void)
{
    m_drawLine = false;
    m_drawSurface = false;
    m_lineWidth = 1.0f;
}

void GlCube::load(float x, float y, float z)
{
    float xh = x/2;
    float yh = y/2;
    float zh = z/2;
    //
    const float tmp[8][3] =
    {
        {+xh,+yh,+zh},
        {-xh,+yh,+zh},
        {-xh,-yh,+zh},
        {+xh,-yh,+zh},
        {+xh,+yh,-zh},
        {-xh,+yh,-zh},
        {-xh,-yh,-zh},
        {+xh,-yh,-zh},
    };
    //
    for(int i=0;i<8;i++)
        for(int j=0;j<3;j++)
            m_vertex[i][j] = tmp[i][j];
    //
}

void GlCube::draw()
{
    const static GLubyte lineIndex[24]=
    {
        0,1,
        1,2,
        2,3,
        3,0,
        4,5,
        5,6,
        6,7,
        7,4,
        0,4,
        1,5,
        2,6,
        3,7
    };
    const static GLubyte surfaceIndex[24]=
    {
        0,1,2,3,
        0,4,5,1,
        1,5,6,2,
        2,6,7,3,
        0,3,7,4,
        4,7,6,5
    };
    const static float SQRT_3 = 1.7320508;
    const static float normal[8][3] =
    {
        {+SQRT_3,+SQRT_3,+SQRT_3},
        {-SQRT_3,+SQRT_3,+SQRT_3},
        {-SQRT_3,-SQRT_3,+SQRT_3},
        {+SQRT_3,-SQRT_3,+SQRT_3},
        {+SQRT_3,+SQRT_3,-SQRT_3},
        {-SQRT_3,+SQRT_3,-SQRT_3},
        {-SQRT_3,-SQRT_3,-SQRT_3},
        {+SQRT_3,-SQRT_3,-SQRT_3},
    };
    //
    if(m_drawLine || m_drawSurface)
    {
        glVertexPointer(3,GL_FLOAT,0,m_vertex);
        glNormalPointer(GL_FLOAT,0,normal);
    }
    if(m_drawLine)
    {
        m_materialLine.useMaterial();
        glLineWidth(m_lineWidth);
        glDrawElements(GL_LINES,24,GL_UNSIGNED_BYTE,lineIndex);
    }
    if(m_drawSurface)
    {
        m_materialsurface.useMaterial();
        glDrawElements(GL_QUADS,24,GL_UNSIGNED_BYTE,surfaceIndex);
    }
}

bool GlCube::setLineWidth(float w)
{
    if(w <= 0)
        return false;
    m_lineWidth = w;
    return true;
}
