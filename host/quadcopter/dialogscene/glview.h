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

#ifndef GLVIEW_H
#define GLVIEW_H

class GlView
{
public:
    GlView();
    void useViewMatrix();
    void changeLongititude(float delta);
    void changeLatitude(float delta);
    void setLongititude(float longititude);
    void setLatitude(float latitude);
    void zoom(float times);
    float getHorizon() const;
    void getViewPoint(float center[3]) const;
    void resize(int width, int height);
    void updateViewPortAndProjection(void);
    void setHorizon(float sizeHalf);
    void setViewPoint(float const p[3]);
    void setViewPoint(float x, float y, float z);
private:
    float m_longititude; // 视角的经度，单位：度。
    float m_latitude; // 视角的纬度，单位：度。
    float m_horizon; // 视野，保证各轴能看到[-m_horizon,m_horizon]的范围。
    int m_viewPortWidth; // 视口宽度。
    int m_viewPortHeight; // 视口高度。
    float m_viewPoint[3]; // 视点。
};

#endif // GLVIEW_H
