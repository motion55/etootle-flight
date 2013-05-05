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
