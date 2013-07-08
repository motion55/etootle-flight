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

#include "glgrid.h"
#include "vector.h"
#include <math.h>

GlGrid::GlGrid()
{
    genericInit();
}

GlGrid::~GlGrid()
{
    destroy();
}

// row和col为格数。
GlGrid::GlGrid(int row,int col)
{
    genericInit();
    make(row,col);
}

void GlGrid::genericInit()
{
    m_row = m_col = 0;
    m_vertex = 0;
    m_normal = 0;
    m_lineWidth = 1;
    m_surfaceIndexArray = 0;
    m_drawLine = m_drawSurface = false;
}

bool GlGrid::isValid()
{
    return (m_row > 0) && (m_col > 0) && (m_vertex != 0) && (m_normal != 0);
}

// 创建网格。
// row和col为顶点数，格数为顶点数-1。
// 返回true表示创建成功。
// 创建成功后，网格的值不确定。
bool GlGrid::make(int row, int col)
{
    if(isValid())
        destroy();
    //
    // 筛参数。
    if(row < 2 || row > MAX_ROW_AND_COL || col < 2 || col > MAX_ROW_AND_COL)
        return false;
    //
    // 获取顶点、法线和编号空间。
    int amount  = row * col;
    m_vertex = new float[amount * 3];
    if(m_vertex == 0)
        return false;
    m_normal = new float[amount * 3];
    if(m_normal == 0)
    {
        delete m_vertex;
        m_vertex = 0;
        return false;
    }
    m_surfaceIndexArray = new GLushort [col*2];
    if(m_surfaceIndexArray == 0)
    {
        delete m_vertex;
        m_vertex = 0;
        delete m_normal;
        m_normal = 0;
        return false;
    }
    //
    // 确定创建成功了。
    m_row = row;
    m_col = col;
    //
    // 计算面的编号。
    for(int i=0;i<col;i++)
    {
        m_surfaceIndexArray[i * 2] = i + col;
        m_surfaceIndexArray[i * 2 + 1] = i;
    }
    //
    return true;
}

void GlGrid::destroy(void)
{
    if(!isValid())
        return;
    //
    delete m_vertex;
    delete m_normal;
    //
    m_vertex = 0;
    m_normal = 0;
    m_row = m_col = 0;
}

void GlGrid::draw(void)
{
    if(!isValid())
        return;
    //
    // 使用顶点和法向数组。
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    {
        if(m_drawLine)
        {
            m_materialLine.useMaterial();
            glLineWidth(m_lineWidth);
            //
            for(int i=0;i<m_row;i++)
            {
                int t = i * m_col * 3;
                glVertexPointer(3,GL_FLOAT,0,&(m_vertex[t]) );
                glNormalPointer(GL_FLOAT,0,&(m_normal[t]) );
                glDrawArrays(GL_LINE_STRIP,0,m_col);
            }
            int t = m_col * 3 * sizeof(float);
            for(int i=0;i<m_col;i++)
            {
                glVertexPointer(3,GL_FLOAT,t,&(m_vertex[3 * i]) );
                glNormalPointer(GL_FLOAT,t,&(m_normal[3 * i]) );
                glDrawArrays(GL_LINE_STRIP,0,m_row);
            }
        }
        if(m_drawSurface)
        {
            m_materialSurface.useMaterial();
            //
            for(int i=0;i<m_row-1;i++)
            {
                glVertexPointer(3,GL_FLOAT,0,&(m_vertex[m_col * i * 3]) );
                glNormalPointer(GL_FLOAT,0,&(m_normal[m_col * i * 3]) );
                glDrawElements(GL_QUAD_STRIP,2*m_col,GL_UNSIGNED_SHORT,m_surfaceIndexArray);
            }
        }
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

// 用顶点计算法向。
void GlGrid::calculateNormal(bool verticalRoll)
{
    int r = m_row;
    int c = m_col;
    int sum = r * c;//sum为顶点数量；
    //
    //计算面的法向，存在左上角顶点上；
    int tmp = (sum - 1) - m_col; // 除了最后一行的顶点数。
    float v1[3],v2[3];
    for(int i=0;i<sum;i++)
    {
        if( (i+1) % c == 0 || i > tmp)//下界和右界置0；
        {
            for(int j=0;j<3;j++)
                m_normal[3*(i) + j] = 0.0f;
            continue;
        }
        //
        for(int j=0;j<3;j++)
        {//i为左上，v1="↘",v2="↗"；
            v1[j] = m_vertex[3*(i	)+ j]  -  m_vertex[3*(i + c + 1)+ j];
            v2[j] = m_vertex[3*(i + c)+ j]  -  m_vertex[3*(i + 1)+ j];
        }
        Vector::cross(v2,v1,&(m_normal[3*(i)]));
        Vector::normalize(&(m_normal[3* (i)]));
        //
    }
    //
    //计算点的法向；
    int flag = 0;
    int k;
    int id,im;
    for(int i=sum-1;i>=0;i--)
    {
        flag = 0;
        //
        id = i / c;
        im = i % c;
        if      (id == 0)   flag += 1;//上界；
        else if	(id == c-1) flag += 4;//下界；
        if      (im == 0)   flag += 2;//左界；
        else if (im == c-1) flag += 8;//右界；
        //
        #define zs ( m_normal[3*(i - c - 1  ) + k] )
        #define zx ( m_normal[3*(i - 1      ) + k] )
        #define yx ( m_normal[3*(i          ) + k] )
        #define ys ( m_normal[3*(i - c      ) + k] )
        #define mb yx//右下为目标；
        //
        switch(flag)
        {
        case 0://中
            for(k=0;k<3;k++)
                mb = yx + ys + zs + zx;
            break;
        case 1://上
            for(k=0;k<3;k++)
                mb = yx + zx;
            break;
        case 2://左
            for(k=0;k<3;k++)
                mb = ys + yx;
            break;
        case 4://下
            for(k=0;k<3;k++)
                mb = zs + ys;
            break;
        case 8://右
            for(k=0;k<3;k++)
                mb = zs + zx;
            break;
        case 3://左上
            for(k=0;k<3;k++)
                mb = yx;
            break;
        case 6://左下
            for(k=0;k<3;k++)
                mb = ys;
            break;
        case 12://右下
            for(k=0;k<3;k++)
                mb = zs;
            break;
        case 9://右上
            for(k=0;k<3;k++)
                mb = zx;
            break;
        }
        //
        #undef zs
        #undef zx
        #undef yx
        #undef ys
        #undef mb
        Vector::normalize( &(m_normal[3*i]) );
        //
    }
    //
    // 如果不需要垂直卷起来，已经结束。
    if(!verticalRoll)
        return;
    //
    float ver1[3],ver2[3],pre1[3],pre2[3],t[3];
    //
    //ver1
    for(int i=0;i<r;i++)
    {
        for(int j=0;j<3;j++)
            t[j] = 0;
        if(i < (r - 1))
        {
            //
            //ver1
            for(int j=0;j<3;j++)
            {//i为左上，v1="↘",v2="↗"；
                v1[j] = m_vertex[3*(i * c	)+ j]  -  m_vertex[3*(i * c + c + 1)+ j];
                v2[j] = m_vertex[3*(i * c + c)+ j]  -  m_vertex[3*(i * c + 1)+ j];
            }
            Vector::cross(v2,v1,ver1);
            Vector::normalize(ver1);
            //
            //ver2
            for(int j=0;j<3;j++)
            {//i为左上，v1="↘",v2="↗"；
                v1[j] = m_vertex[3*(c * i + c - 2		)+ j]  -  m_vertex[3*(c * i + 2 * c - 1	)+ j];
                v2[j] = m_vertex[3*(c * i + 2 * c - 2	)+ j]  -  m_vertex[3*(c * i + c - 1		)+ j];
            }
            //
            Vector::cross(v2,v1,ver2);
            Vector::normalize(ver2);
            //
            //
            for(int j=0;j<3;j++)
                t[j] = ver1[j] + ver2[j];
        }
        if(i != 0)
            for(int j=0;j<3;j++)
                t[j] += pre1[j] + pre2[j];
        //
        Vector::normalize(t);
        //
        for(int j=0;j<3;j++)
            m_normal[3*(i * c) + j] = m_normal[3*(i * c + c - 1) + j] = t[j];
        //
        for(int j=0;j<3;j++)
        {
            pre1[j] = ver1[j];
            pre2[j] = ver2[j];
        }
    }
}

// 把网格变成带缺口的圆台侧面。Z轴为母线。
// height为高度，实数范围，正就在Z正方向，负就在Z负方向，0就在XOY平面。
// radiusBottom为底半径,radiusTop为顶半径，必须非负。
// angle为角度，0~2*M_PI，截取；
// angleBegin为开始角度，绕Z轴，从X轴起算逆时针。
bool GlGrid::loadCylinder(float height, float radiusBottom, float radiusTop, float angle, float angleBegin)
{
    if(!isValid())
        return false;
    if(radiusBottom < 0.0f || radiusTop < 0.0f)
        return false;
    if(m_row < 2 || m_col < 4)
        return false;
    //
    while(angleBegin > 2*M_PI)
        angleBegin -= 2*M_PI;
    while(angleBegin < 0)
        angleBegin += 2*M_PI;
    if(angle > 2*M_PI)
        angle = 2*M_PI;
    if(angle < 0.0f)
        angle = 0.0f;
    //
    // 设置Z。
    float heiDelta = height / (m_row-1);
    float hei = 0.0f;
    for(int i=0;i<m_row;i++,hei+=heiDelta)
        for(int j=0;j<m_col;j++)
            m_vertex[(i*m_col+j) * 3 + 2] = hei;
    //
    // 设置XY。
    float ang = angleBegin;
    float angDelta = angle / (m_col-1);
    for(int j=0;j<m_col;j++,ang+=angDelta)
    {
        float rad = radiusBottom;
        float radDelta = (radiusTop - radiusBottom) / (m_row - 1);
        for(int i=0;i<m_row;i++,rad+=radDelta)
        {
            m_vertex[(i*m_col+j) * 3    ] = cosf(ang) * rad;
            m_vertex[(i*m_col+j) * 3 + 1] = sinf(ang) * rad;
        }
    }
    //
    // 计算点法向。
    if(angle < 2*M_PI-TOLERANCE)
        calculateNormal(false);
    else
        calculateNormal(true);
    //
    return true;
}

bool GlGrid::setLineWidth(float w)
{
    if(w <= 0)
        return false;
    m_lineWidth = w;
    return true;
}

// 加载椭球面。
// radiusX、radiusY、radiusZ为各轴半轴长，必须为正。
// angleBeginXY和angleEndXY为与XY平面的开始、结束角，[-M_PI/2,M_PI/2]。
//     angleBeginXY > angleEndXY时，加载椭球外表面。
//     angleBeginXY < angleEndXY时，加载椭球内表面。
//     angleBeginXY和angleEndXY不可以相等。
// angleBeginZ和angleEndZ为绕Z轴的开始、结束角，逆时针，可以跨过0，不可以相等 。
bool GlGrid::LoadSphere(float radiusX,float radiusY,float radiusZ,
                        float angleBeginXY,float angleEndXY,float angleBeginZ,float angleEndZ)
{
    if(radiusX <= 0.0f || radiusY <= 0.0f || radiusZ <= 0.0f)
        return false;
    if(m_row < 3 || m_col < 4)
        return false;
    //
    if(angleBeginXY < -M_PI/2-TOLERANCE || angleBeginXY > M_PI/2+TOLERANCE
            || angleEndXY < -M_PI/2-TOLERANCE || angleEndXY > M_PI/2+TOLERANCE)
        return false;
    //
    while(angleBeginZ > 2*M_PI+TOLERANCE)
        angleBeginZ -= 2*M_PI;
    while(angleBeginZ < 0)
        angleBeginZ += 2*M_PI;
    while(angleEndZ > 2*M_PI+TOLERANCE)
        angleEndZ -= 2*M_PI;
    while(angleEndZ < 0)
        angleEndZ += 2*M_PI;
    //
    // 保证angleEndZ在angleBeginZ前面。
    if(angleEndZ < angleBeginZ)
        angleEndZ += 2*M_PI;
    //
    // 开始。
    float angXY = angleBeginXY;
    float angXYdelta = (angleEndXY - angleBeginXY) / (m_row - 1);
    for(int i=0;i<m_row;i++,angXY+=angXYdelta)
    {
        // 设置Z。
        float z = radiusZ * sinf(angXY);
        for(int j=0;j<m_col;j++)
            m_vertex[(i*m_col+j) * 3 + 2] = z;
        //
        // XY。
        float tmp = cosf(angXY);
        float rX = radiusX * tmp;
        float rY = radiusY * tmp;
        //
        float angZ = angleBeginZ;
        float angZdelta = (angleEndZ - angleBeginZ) / (m_col - 1);
        for(int j=0;j<m_col;j++,angZ+=angZdelta)
        {
            m_vertex[(i*m_col+j) * 3    ] = rX * cosf(angZ);
            m_vertex[(i*m_col+j) * 3 + 1] = rY * sinf(angZ);
        }
    }
    //
    // 计算点法向。
    if(angleEndZ-angleBeginZ < 2*M_PI-TOLERANCE)
        calculateNormal(false);
    else
        calculateNormal(true);
    //
    return true;
}

// rowIndex、colIndex为顶点索引，[0,m_row-1]，[0,m_col-1]。
bool GlGrid::setVertex(int rowIndex, int colIndex, const float val[3])
{
    if(rowIndex < 0 || colIndex < 0 || rowIndex >= m_row || colIndex >= m_col)
        return false;
    //
    float * to = &m_vertex[(colIndex * m_col + rowIndex)*3];
    to[0] = val[0];
    to[1] = val[1];
    to[2] = val[2];
    //
    return true;
}

// rowIndex、colIndex为顶点索引，[0,m_row-1]，[0,m_col-1]。
bool GlGrid::setNormal(int rowIndex, int colIndex, const float val[])
{
    if(rowIndex < 0 || colIndex < 0 || rowIndex >= m_row || colIndex >= m_col)
        return false;
    //
    float * to = &m_normal[(colIndex * m_col + rowIndex)*3];
    to[0] = val[0];
    to[1] = val[1];
    to[2] = val[2];
    //
    return true;
}
