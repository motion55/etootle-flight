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

#ifndef MATERIAL_H
#define MATERIAL_H

class Material
{
public:
    Material();
    virtual ~Material();
    void useMaterial();
    void moreTransparency(float factor=1.0f);
    void setTransparency(float alpha=1.0f);
    void loadColor(float r,float g,float b, float a=1.0);
    void loadColorRed()   {loadColor(1,0,0);}
    void loadColorGreen() {loadColor(0,1,0);}
    void loadColorBlue()  {loadColor(0,0,1);}
    void loadColorBlack() {loadColor(0,0,0);}
    void loadColorWhite() {loadColor(1,1,1);}
    void loadBlackPlastic();
    void loadWhitePlastic();
    void loadCopper();
    void loadBrass();
    void loadBronze();
    void loadChrome();
    void loadGold();
    void loadWhiteWax();
    void loadSiver();
    void loadJaspure();
    void loadPearl();
    void loadRuby();
    void loadBeryl();
    void loadViolet();
protected:
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_shininess;
};

#endif // MATERIAL_H
