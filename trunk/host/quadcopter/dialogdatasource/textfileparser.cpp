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

#include "textfileparser.h"
#include "protocol.h"
#include <QTextStream>

TextFileParser::TextFileParser()
{
    m_file = NULL;
    m_timer = NULL;
}

TextFileParser::~TextFileParser()
{
}

// 成功就返回true，否则false。
// name:文件名，period:时标周期，ms。
bool TextFileParser::start(const QString &file_name,int period_ms)
{
    if(m_timer != NULL || m_file != NULL)
        stop();
    //
    m_file = new QFile(file_name);
    if(! m_file->exists() || ! m_file->open(QFile::ReadOnly | QIODevice::Text))
        return false;
    //
    m_timer = new QTimer(this);
    m_timer->setInterval(period_ms);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(periodPass()));
    m_timer->start();
    //
    return true;
}

void TextFileParser::pause()
{
    if(m_timer != NULL)
        m_timer->stop();
}

void TextFileParser::continue_()
{
    if(m_timer != NULL)
        m_timer->start();
}

void TextFileParser::stop()
{
    if(m_timer != NULL)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = NULL;
    }
    if(m_file != NULL)
    {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }
    //
    emit onStop();
}

// 不断解析，直到遇到时标。
void TextFileParser::periodPass()
{
    int type = 0;
    do
    {
        QByteArray buf = m_file->readLine();
        if(buf.length() <= 0)
        {
            stop();
            return;
        }
        //
        QTextStream ts(buf);
        ts >> type;
        switch(type)
        {
        case Protocol::RETURN_ATTITUDE_QUATERNION :
            {
            float w,x,y,z;
            ts >> w >> x >> y >> z;
                Quaternion atti(w,x,y,z);
                emit displayAttitudeQuaternion(atti);
            }
            break;
        case Protocol::RETURN_TRANSLATION :
            {
                float x,y,z;
                ts >> x >> y >> z;
                emit displayTranslation(x,y,z);
            }
            break;
        }
    }while(type != Protocol::PERIOD_MARK);
}
