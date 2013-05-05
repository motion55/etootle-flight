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

#include "glquadcopter.h"

GlQuadcopter::GlQuadcopter(float scale)
{
    //原来的单位是米，乘上scale后再加载模型。
    m_scale = scale;
    //
    m_body.load(BODY_X*scale,BODY_Y*scale,BODY_Z*scale);
    m_body.getMaterialLine()->loadColorBlack();
    m_body.getMaterialSurface()->loadColor(0.0,0.7,0.8,0.9);
    m_body.needDrawSurface();
    m_body.needDrawLine();
    //
    m_arm.make(2,9);
    m_arm.loadCylinder(ARM_LENGTH*scale,ARM_RADIUS*scale,ARM_RADIUS*scale);
    m_arm.getMaterialSurface()->loadBlackPlastic();
    m_arm.needDrawSurface();
    //
    m_motor.make(2,13);
    m_motor.loadCylinder(MOTOR_HEIGHT*scale,MOTOR_RADIUS*scale,MOTOR_RADIUS*scale);
    m_motor.getMaterialSurface()->loadBronze();
    m_motor.needDrawSurface();
    //
    m_airscrew.make(11,31);
    m_airscrew.LoadSphere(AIRSCREW_RADIUS*scale,AIRSCREW_RADIUS*scale,AIRSCREW_HEIGHT*scale);
    m_airscrew.getMaterialSurface()->loadPearl();
    m_airscrew.getMaterialSurface()->moreTransparency(0.75);
    m_airscrew.needDrawSurface();
}

void GlQuadcopter::draw()
{
    m_body.draw();
    //
    glPushMatrix();
    {
        //glRotatef(45,0,0,1);
        //
        glRotatef(90,0,1,0);
        m_arm.getMaterialSurface()->loadWhitePlastic();
        m_arm.draw();
        glRotatef(90,1,0,0);
        m_arm.draw();
        glRotatef(90,1,0,0);
        m_arm.getMaterialSurface()->loadBlackPlastic();
        m_arm.draw();
        glRotatef(90,1,0,0);
        m_arm.draw();
    }glPopMatrix();
    //
    glPushMatrix();
    {
        //glRotatef(45,0,0,1);
        //
        glTranslatef(0,0,-(AIRSCREW_HEIGHT/2)*m_scale);
        float offset = ARM_LENGTH*m_scale;
        glTranslatef(offset,0,0);
        m_motor.draw();
        glTranslatef(-offset,offset,0);
        m_motor.draw();
        glTranslatef(-offset,-offset,0);
        m_motor.draw();
        glTranslatef(offset,-offset,0);
        m_motor.draw();
    }glPopMatrix();
    //
    glPushMatrix();
    {
        //glRotatef(45,0,0,1);
        //
        glTranslatef(0,0,(BODY_Z+AIRSCREW_HEIGHT/2)*m_scale);
        float offset = ARM_LENGTH*m_scale;
        glTranslatef(offset,0,0);
        m_airscrew.draw();
        glTranslatef(-offset,offset,0);
        m_airscrew.draw();
        glTranslatef(-offset,-offset,0);
        m_airscrew.draw();
        glTranslatef(offset,-offset,0);
        m_airscrew.draw();
    }glPopMatrix();
}
