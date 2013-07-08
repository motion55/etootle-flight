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

#include "material.h"
#include <GL/gl.h>

Material::Material()
{
    for(int i=0;i<3;i++)
        m_ambient[i] = m_diffuse[i]=m_specular[i]=0.0f;
    m_ambient[3] = m_diffuse[3]=m_specular[3]=1.0f;
    m_shininess = 0.0f;
}

Material::~Material()
{
}

void Material::useMaterial()
{
    glMaterialfv(GL_FRONT,GL_AMBIENT,m_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,m_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,m_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,&m_shininess);
}

void Material::moreTransparency(float factor)
{
    m_ambient[3]  *= factor;
    m_diffuse[3]  *= factor;
    m_specular[3] *= factor;
}

void Material::setTransparency(float alpha)
{
    m_ambient[3]  = alpha;
    m_diffuse[3]  = alpha;
    m_specular[3] = alpha;
}

void Material::loadColor(float r, float g, float b,float a)
{
    for(int i=0;i<3;i++)
        m_diffuse[i]=m_specular[i]=0.0f;
    m_ambient[0] = r;
    m_ambient[1] = g;
    m_ambient[2] = b;
    m_shininess = 0.0f;
    //
    m_ambient[3] = m_diffuse[3]=m_specular[3] = a;
}

// 加载材质，黑色塑料。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadBlackPlastic()
{
    for(int i=0;i<3;i++)
    {
        m_ambient[i] = 0.0f;
        m_diffuse[i] = 0.10f;
        m_specular[i] = 0.5f;
    }
    m_ambient[3] = 1.0f;
    m_diffuse[3] = 1.0f;
    m_specular[3] = 1.0f;
    //
    m_shininess = 32.0f;
}

// 加载材质，白色塑料。
void Material::loadWhitePlastic()
{
    for(int i=0;i<3;i++)
    {
        m_ambient[i] = 0.0f;
        m_diffuse[i] = 0.90f;
        m_specular[i] = 0.5f;
    }
    m_ambient[3] = 1.0f;
    m_diffuse[3] = 1.0f;
    m_specular[3] = 1.0f;
    //
    m_shininess = 32.0f;
}

// 加载材质，铜。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadCopper()
{
    float a[4] = {0.191250, 0.073500, 0.022500, 1.000000};
    float d[4] = {0.703800, 0.270480, 0.082800, 1.000000};
    float s[4] = {0.256777, 0.137622, 0.086014, 1.000000};
    m_shininess = 12.800000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，黄铜。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadBrass()
{
    float a[4] = {0.329412, 0.223529, 0.027451, 1.000000};
    float d[4] = {0.780392, 0.568627, 0.113725, 1.0000000};
    float s[4] = {0.992157, 0.941176, 0.807843, 1.000000};
    m_shininess = 27.897400;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，青铜。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadBronze()
{
    float a[4] = {0.212500, 0.127500, 0.054000, 1.000000};
    float d[4] = {0.714000, 0.428400, 0.181440, 1.000000};
    float s[4] = {0.393548, 0.271906, 0.166721, 1.000000};
    m_shininess = 25.600000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，铬。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadChrome()
{
    float a[4] = {0.250000, 0.250000, 0.250000, 1.000000};
    float d[4] = {0.400000, 0.400000, 0.400000, 1.000000};
    float s[4] = {0.774597, 0.774597, 0.774597, 1.000000};
    m_shininess = 76.800003;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，金。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadGold()
{
    float a[4] = {0.247250, 0.199500, 0.074500, 1.000000};
    float d[4] = {0.751640, 0.606480, 0.226480, 1.000000};
    float s[4] = {0.628281, 0.555802, 0.366065, 1.000000};
    m_shininess = 51.200001;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，白蜡。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadWhiteWax()
{
    float a[4] = {0.105882, 0.058824, 0.113725, 1.000000};
    float d[4] = {0.427451, 0.470588, 0.541176, 1.000000};
    float s[4] = {0.333333, 0.333333, 0.521569, 1.000000};
    m_shininess = 9.846150;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，银。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadSiver()
{
    float a[4] = {0.192250, 0.192250, 0.192250, 1.000000};
    float d[4] = {0.507540, 0.507540, 0.507540, 1.000000};
    float s[4] = {0.508273, 0.508273, 0.508273, 1.000000};
    m_shininess = 51.200001;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，碧玉。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadJaspure()
{
    float a[4] = {0.135000, 0.222500, 0.157500, 0.950000};
    float d[4] = {0.540000, 0.890000, 0.630000, 0.950000};
    float s[4] = {0.316228, 0.316228, 0.316228, 0.950000};
    m_shininess = 12.800000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，珍珠。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadPearl()
{
    float a[4] = {0.250000, 0.207250, 0.207250, 0.922000};
    float d[4] = {1.000000, 0.829000, 0.829000, 0.922000};
    float s[4] = {0.296648, 0.296648, 0.296648, 0.922000};
    m_shininess = 11.264000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，红宝石。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadRuby()
{
    float a[4] = {0.174500, 0.011750, 0.011750, 0.550000};
    float d[4] = {0.614240, 0.041360, 0.041360, 0.550000};
    float s[4] = {0.727811, 0.626959, 0.626959, 0.550000};
    m_shininess = 76.800003;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，绿宝石、绿松石。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadBeryl()
{
    float a[4] = {0.100000, 0.187250, 0.174500, 0.800000};
    float d[4] = {0.396000, 0.741510, 0.691020, 0.800000};
    float s[4] = {0.297254, 0.308290, 0.306678, 0.800000};
    m_shininess = 12.800000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}

// 加载材质，紫罗兰。
// http://blog.sina.com.cn/s/blog_7590da79010172vn.html
void Material::loadViolet()
{
    float a[4] = {0.110000, 0.060000, 0.090000, 1.000000};
    float d[4] = {0.430000, 0.470000, 0.540000, 1.000000};
    float s[4] = {0.330000, 0.330000, 0.520000, 1.000000};
    m_shininess = 22.000000;
    //
    for(int i=0;i<4;i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }
}
