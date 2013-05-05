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


#ifndef _POSIX_QEXTSERIALPORT_H_
#define _POSIX_QEXTSERIALPORT_H_

#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include "qextserialbase.h"

class Posix_QextSerialPort:public QextSerialBase {
public:
    Posix_QextSerialPort();
    Posix_QextSerialPort(const Posix_QextSerialPort& s);
    Posix_QextSerialPort(const QString & name);
    Posix_QextSerialPort(const PortSettings& settings);
    Posix_QextSerialPort(const QString & name, const PortSettings& settings);
    Posix_QextSerialPort& operator=(const Posix_QextSerialPort& s);
    virtual ~Posix_QextSerialPort();

    virtual void setBaudRate(BaudRateType);
    virtual void setDataBits(DataBitsType);
    virtual void setParity(ParityType);
    virtual void setStopBits(StopBitsType);
    virtual void setFlowControl(FlowType);
    virtual void setTimeout(ulong, ulong);

    virtual bool open(OpenMode mode=0);
    virtual void close();
    virtual void flush();

    virtual qint64 size() const;
    virtual qint64 bytesAvailable();

    virtual void ungetChar(char c);

    virtual void translateError(ulong error);

    virtual void setDtr(bool set=true);
    virtual void setRts(bool set=true);
    virtual ulong lineStatus();

protected:
    QFile* Posix_File;
    struct termios Posix_CommConfig;
    struct timeval Posix_Timeout;
    struct timeval Posix_Copy_Timeout;

    virtual qint64 readData(char * data, qint64 maxSize);
    virtual qint64 writeData(const char * data, qint64 maxSize);
};

#endif
