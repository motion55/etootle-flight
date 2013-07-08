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

#ifndef GLQUADCOPTER_H
#define GLQUADCOPTER_H

#include "globject.h"
#include "glcube.h"
#include "glgrid.h"

class GlQuadcopter : public GlObject
{
public:
    explicit GlQuadcopter(float scale=1.0f);
    virtual void draw(void);
private:
    GlGrid m_arm;
    GlGrid m_motor;
    GlGrid m_airscrew;
    GlCube m_body;
    float m_scale;
    const static float BODY_X = 0.090;
    const static float BODY_Y = 0.060;
    const static float BODY_Z = 0.025;
    const static float ARM_RADIUS = 0.010;
    const static float ARM_LENGTH = 0.225;
    const static float MOTOR_RADIUS = 0.015;
    const static float MOTOR_HEIGHT = 0.040;
    const static float AIRSCREW_RADIUS = 0.127;
    const static float AIRSCREW_HEIGHT = 0.010;

};

#endif // GLQUADCOPTER_H
