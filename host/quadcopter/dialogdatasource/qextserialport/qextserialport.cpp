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


/*!
\class QextSerialPort
\version 1.0.0
\author Stefan Sander

A cross-platform serial port class.
This class encapsulates a serial port on both POSIX and Windows systems.  The user will be
notified of errors and possible portability conflicts at run-time by default - this behavior can
be turned off by defining _TTY_NOWARN_ (to turn off all warnings) or _TTY_NOWARN_PORT_ (to turn
off portability warnings) in the project.

\note
On Windows NT/2000/XP this class uses Win32 serial port functions by default.  The user may
select POSIX behavior under NT, 2000, or XP ONLY by defining _TTY_POSIX_ in the project. I can
make no guarantees as to the quality of POSIX support under NT/2000 however.

*/

#include <stdio.h>
#include "qextserialport.h"

/*!
\fn QextSerialPort::QextSerialPort()
Default constructor.  Note that the naming convention used by a QextSerialPort constructed with
this constructor will be determined by #defined constants, or lack thereof - the default behavior
is the same as _TTY_LINUX_.  Possible naming conventions and their associated constants are:

\verbatim

Constant         Used By         Naming Convention
----------       -------------   ------------------------
_TTY_WIN_        Windows         COM1, COM2
_TTY_IRIX_       SGI/IRIX        /dev/ttyf1, /dev/ttyf2
_TTY_HPUX_       HP-UX           /dev/tty1p0, /dev/tty2p0
_TTY_SUN_        SunOS/Solaris   /dev/ttya, /dev/ttyb
_TTY_DIGITAL_    Digital UNIX    /dev/tty01, /dev/tty02
_TTY_FREEBSD_    FreeBSD         /dev/ttyd0, /dev/ttyd1
_TTY_LINUX_      Linux           /dev/ttyS0, /dev/ttyS1
<none>           Linux           /dev/ttyS0, /dev/ttyS1
\endverbatim

The object will be associated with the first port in the system, e.g. COM1 on Windows systems.
See the other constructors if you need to use a port other than the first.
*/
QextSerialPort::QextSerialPort()
 : QextBaseType()
{}

/*!
\fn QextSerialPort::QextSerialPort(const QString & name)
Constructs a serial port attached to the port specified by name.
name is the name of the device, which is windowsystem-specific,
e.g."COM1" or "/dev/ttyS0".
*/
QextSerialPort::QextSerialPort(const QString & name)
 : QextBaseType(name)
{}

/*!
\fn QextSerialPort::QextSerialPort(PortSettings const& settings)
Constructs a port with default name and settings specified by the settings parameter.
*/
QextSerialPort::QextSerialPort(PortSettings const& settings)
 : QextBaseType(settings)
{}

/*!
\fn QextSerialPort::QextSerialPort(const QString & name, PortSettings const& settings)
Constructs a port with the name and settings specified.
*/
QextSerialPort::QextSerialPort(const QString & name, PortSettings const& settings)
 : QextBaseType(name, settings)
{}

/*!
\fn QextSerialPort::QextSerialPort(const QextSerialPort& s)
Copy constructor.
*/
QextSerialPort::QextSerialPort(const QextSerialPort& s)
 : QextBaseType(s)
{}

/*!
\fn QextSerialPort& QextSerialPort::operator=(const QextSerialPort& s)
Overrides the = operator.
*/
QextSerialPort& QextSerialPort::operator=(const QextSerialPort& s)
{
    return (QextSerialPort&)QextBaseType::operator=(s);
}

/*!
\fn QextSerialPort::~QextSerialPort()
Standard destructor.
*/
QextSerialPort::~QextSerialPort()
{}
