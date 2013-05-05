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

#ifndef _WIN_QEXTSERIALPORT_H_
#define _WIN_QEXTSERIALPORT_H_

#include "qextserialbase.h"

/*if all warning messages are turned off, flag portability warnings to be turned off as well*/
#ifdef _TTY_NOWARN_
#define _TTY_NOWARN_PORT_
#endif

#include <windows.h>

class Win_QextSerialPort:public QextSerialBase {
public:
    Win_QextSerialPort();
    Win_QextSerialPort(Win_QextSerialPort const& s);
    Win_QextSerialPort(const QString & name);
    Win_QextSerialPort(const PortSettings& settings);
    Win_QextSerialPort(const QString & name, const PortSettings& settings);
    Win_QextSerialPort& operator=(const Win_QextSerialPort& s);
    virtual ~Win_QextSerialPort();
    virtual bool open(OpenMode mode=0);
    virtual void close();
    virtual void flush();
    virtual qint64 size() const;
    virtual void ungetChar(char c);
    virtual void setFlowControl(FlowType);
    virtual void setParity(ParityType);
    virtual void setDataBits(DataBitsType);
    virtual void setStopBits(StopBitsType);
    virtual void setBaudRate(BaudRateType);
    virtual void setDtr(bool set=true);
    virtual void setRts(bool set=true);
    virtual ulong lineStatus(void);
    virtual qint64 bytesAvailable();
    virtual void translateError(ulong);
    virtual void setTimeout(ulong, ulong);

protected:
    HANDLE Win_Handle;
    COMMCONFIG Win_CommConfig;
    COMMTIMEOUTS Win_CommTimeouts;

    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);
};

#endif
