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

#include "binaryparser.h"
#include "protocol.h"
#include "mymath.h"
#include "mainwindow.h"

BinaryParser::BinaryParser()
{
    m_status = FINDING_55;
    m_frameLength = 0;
    m_checkCrc = true;
    m_needPrintTransmitPacket = false;
    m_needPrintReceivePacket = false;
}

void BinaryParser::needCheckCrc(bool need)
{
    m_checkCrc = need;
}

void BinaryParser::needPrintTransmitPacket(bool need)
{
    m_needPrintTransmitPacket = need;
}

void BinaryParser::needPrintReceivePacket(bool need)
{
    m_needPrintReceivePacket = need;
}

void BinaryParser::onReceivedData(const QByteArray &bytes)
{
    int len = bytes.length();
    for(int i=0;i<len;i++)
        handleOndByte((uint8_t)bytes[i]);
}

void BinaryParser::handleOndByte(int byte)
{
    if(m_status == FINDING_55)
    {
        // 看是否在找帧开始符，0x55。
        if(byte == 0x55)
            m_status = NEED_AA;
    }
    else if(m_status == NEED_AA)
    {
        // 找到0x55后，下一个必须是0xAA，否则在次寻找0x55。
        if(byte == 0xAA)
            m_status = NEED_LENGTH;
        else
            m_status = FINDING_55;
    }
    else if(m_status == NEED_LENGTH)
    {
        // 0xAA后接帧内容长度，检查长度是否小于DATA_LEN_MAX。
        if(byte < DATA_LEN_MAX)
        {
            m_frameLength = byte;
            m_status = DATA_START;
        }
        else
        {
            // 帧错误，寻找下一帧。
            m_status = FINDING_55;
        }
    }
    // [0,DATA_LEN_MAX-1]就表示正在读数据。
    else if(m_status < DATA_LEN_MAX)
    {
        m_buff[m_status] = byte;
        m_status ++;
        //
        // 判断读完没有。
        if(m_status == m_frameLength)
        {
            //
            // 帧接收完了，处理。
            frameCompleted(m_buff,m_frameLength);
            //
            // 寻找下一帧。
            m_status = FINDING_55;
        }
    }
    // 其他情况不会出现，为防万一，把状态回到"寻找下一帧"。
    else
    {
        m_status = FINDING_55;
    }
}

void BinaryParser::frameCompleted(const char frame[], int len)
{
    // 版本必须为0。
    if(frame[0] != 0)
        return;
    if(len < 4)
        return;
    if(!checkFrameCrc(frame,len))
        return;
    //
    // 判断是否输出包内容。
    if(m_needPrintReceivePacket)
    {
        QString str = QString::fromUtf8("接收 &gt;&gt;");
        QString tmp;
        for(int i=0;i<len;i++)
            str.append(tmp.sprintf(" %.2X",frame[i]&0xFF));
        MainWindow::printMessage(str);
    }
    //
    const uint8_t * param = (const uint8_t *)(frame +2);
    int param_len = len - 4;
    //
    switch((int)(frame[1]))
    {
    case Protocol::RETURN_ATTITUDE_QUATERNION :
        {
            if(param_len < 4*4)
                break;
            //
            // 假设CPU为小端。
            Quaternion atti((float *)param);
            atti.nomalize();
            emit displayAttitudeQuaternion(atti);
        }
        break;
    case Protocol::RETURN_TRANSLATION :
        {
            if(param_len < 3*4)
                break;
            //
            // 假设CPU为小端。
            float * tran = (float *)param;
            emit displayTranslation(tran[0],tran[1],tran[2]);
        }
        break;
    case Protocol::RETURN_ATTITUDE_ANGLE :
        {
            if(param_len < 3*4)
                break;
            //
            float * angle = (float *)param;
            Quaternion atti(angle[0],angle[1],angle[2]);
            emit displayAttitudeQuaternion(atti);
        }
        break;
    case Protocol::RETURN_VECTOR :
        {
            if(param_len < 1+3*4)
                break;
            //
            // 假设CPU为小端。
            float * xyz = (float *)(param+1);
            emit onReturnVector(param[0],xyz[0],xyz[1],xyz[2]);
        }
        break;
    case Protocol::RETURN_STATUS :
        {
            if(param_len < 1+1)
                break;
            //
            QVector<uint8_t> v;
            for(int i=1;i<param_len;i++)
                v.append(param[i]);
            emit onReturnStatus(param[0],v);
        }
        break;
    case Protocol::RETURN_MESSAGE :
        {
            if(param_len < 1)
                break;
            //
            emit onReturnMessage(QString::fromUtf8(
                (const char *)param,param_len));
        }
        break;
    case Protocol::BOOTLOADER_STATUS :
        {
            if(param_len < 1)
                break;
            //
            emit onBootloaderStatus(param[0],QByteArray((const char *)(param+1),param_len-1));
        }
        break;
    case Protocol::PARAMETER :
        {
            if(param_len < 1)
                break;
            //
            emit onParameter(QByteArray((const char *)param,param_len));
        }
        break;
    case Protocol::LOCK_ATTITUDE :
        {
            if(param_len < 1)
                break;
            emit onControlLockAttitude(QByteArray((const char *)param,param_len));
        }
        break;
    }
}

// true:crc16-ccitt为0。
bool BinaryParser::checkFrameCrc(const char frame[], int len)
{
    if(!m_checkCrc)
        return true;
    return MyMath::crc16(0,frame,len) == 0;
}

void BinaryParser::transmitFrame(uint8_t protocol, uint8_t type, const void * data, uint8_t len)
{
    const uint8_t frame[] = {0x55,0xAA,len+4,protocol,type};
    QByteArray b;
    b.append((char*)frame,sizeof(frame));
    b.append((char *)data,len);
    uint16_t crc = MyMath::crc16(0,&protocol,1);
    crc = MyMath::crc16(crc,&type,1);
    crc = MyMath::crc16(crc,data,len);
    b.append((uint8_t)(crc >> 8));
    b.append((uint8_t)crc);
    //
    // 判断是否输出包内容。
    if(m_needPrintTransmitPacket)
    {
        QString str = QString::fromUtf8("发送 &lt;&lt;");
        QString tmp;
        int print_len = b.size()-3; // 3:去掉帧头，下同。
        for(int i=0;i<print_len;i++)
            str.append(tmp.sprintf(" %.2X",b.at(3+i)&0xFF));
        MainWindow::printMessage(str);
    }
    //
    emit send(b);
}

void BinaryParser::cmd_getAttitudeQuaternion()
{
    transmitFrame(Protocol::VERSION,Protocol::GET_ATTITUDE_QUATERNION,0,0);
}

void BinaryParser::cmd_getTranslation()
{
    transmitFrame(Protocol::VERSION,Protocol::GET_TRANSLATION,0,0);
}

void BinaryParser::cmd_getAttitudeAngle()
{
    transmitFrame(Protocol::VERSION,Protocol::GET_ATTITUDE_ANGLE,0,0);
}

void BinaryParser::control_lockAttitude(const QByteArray &param)
{
    transmitFrame(Protocol::VERSION,Protocol::LOCK_ATTITUDE,param.constData(),param.size());
}

void BinaryParser::control_setMode(uint8_t mode)
{
    transmitFrame(Protocol::VERSION,Protocol::SET_CONTROL_MODE,&mode,sizeof(mode));
}

void BinaryParser::control_lockThrottle_setThrottle(const QVector<float> & throttle)
{
    if(throttle.size() != 4)
        return;
    transmitFrame(Protocol::VERSION,Protocol::LOCK_THROTTLE_SET_THROTTLE
                  ,throttle.constData(),throttle.size()*sizeof(float));
}

void BinaryParser::control_sendHeartbeat()
{
    transmitFrame(Protocol::VERSION,Protocol::HEARTBEAT_SEND,0,0);
}

void BinaryParser::cmd_getVector(uint8_t type)
{
    transmitFrame(Protocol::VERSION,Protocol::GET_VECTOR,&type,sizeof(type));
}

void BinaryParser::cmd_getStatus(const QVector<uint8_t> &which)
{
    transmitFrame(Protocol::VERSION,Protocol::GET_STATUS,which.constData(),which.size());
}

void BinaryParser::cmd_bootloadCmd(uint8_t cmd, QByteArray param)
{
    QByteArray t;
    t.append(cmd);
    t.append(param);
    transmitFrame(Protocol::VERSION,Protocol::BOOTLOADER_CMD,t.constData(),t.size());
}

void BinaryParser::cmd_parameter(const QByteArray &param)
{
    transmitFrame(Protocol::VERSION,Protocol::PARAMETER,param.constData(),param.size());
}
