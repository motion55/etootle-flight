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

#ifndef GLGRID_H
#define GLGRID_H

#include "globject.h"
#include "material.h"
#include <math.h>
#include <GL/gl.h>

class GlGrid : public GlObject
{
public:
    static const int MAX_ROW_AND_COL = 4096;
public:
    GlGrid();
    ~GlGrid();
    GlGrid(int row,int col);
    bool isValid();
    bool make(int row,int col);
    void destroy(void);
    virtual void draw(void);
    bool loadCylinder(float height, float radiusBottom, float radiusTop,
                      float angle=2*M_PI, float angleBegin=0.0f);
    Material * getMaterialLine() {return &m_materialLine;}
    Material * getMaterialSurface() {return &m_materialSurface;}
    bool setLineWidth(float w);
    void needDrawLine(bool yes=true)    {m_drawLine = yes;}
    void needDrawSurface(bool yes=true) {m_drawSurface = yes;}
    bool LoadSphere(float radiusX, float radiusY, float radiusZ,
                    float angleBeginXY=-M_PI/2, float angleEndXY=M_PI/2,
                    float angleBeginZ=0.0f, float angleEndZ=2*M_PI);
    bool setVertex(int rowIndex,int colIndex,float const val[3]);
    bool setNormal(int rowIndex,int colIndex,float const val[3]);
protected:
    int m_row,m_col; //顶点数，格数为顶点数-1。
    float * m_vertex;
    float * m_normal;
    bool m_drawLine,m_drawSurface;
    float m_lineWidth;
    GLushort * m_surfaceIndexArray;
    Material m_materialLine;
    Material m_materialSurface;
private:
    const static float TOLERANCE = 1e-4;
    void genericInit();
    void calculateNormal(bool verticalRoll=false);
};

#endif // GLGRID_H
