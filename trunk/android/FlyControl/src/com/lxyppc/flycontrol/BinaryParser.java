package com.lxyppc.flycontrol;


import java.io.UnsupportedEncodingException;

import android.util.Log;

public class BinaryParser {
	private     static final int FINDING_55   = 100;
	private static final int NEED_AA      = 101;
	private static final int NEED_LENGTH  = 102;
	private static final int DATA_LEN_MAX = 32;
	private static final int DATA_START   = 0;
	private int m_status;
	private int m_frameLength;
	private byte[] m_buff = new byte[DATA_LEN_MAX];
	private boolean m_checkCrc;
	private boolean m_needPrintTransmitPacket;
	private boolean m_needPrintReceivePacket;
	private Signals emit = new Signals();
	private static final String TAG = "FLYC";
	
	BinaryParser(Signals signal){
	    m_status = FINDING_55;
	    m_frameLength = 0;
	    m_checkCrc = false;
	    m_needPrintTransmitPacket = false;
	    m_needPrintReceivePacket = false;
	    emit = signal;
	}
	
	void needCheckCrc(){
		needCheckCrc(true);
	}
	
	void needCheckCrc(boolean need){
	    m_checkCrc = need;
	}
	
	void needPrintTransmitPacket(boolean need)
	{
	    m_needPrintTransmitPacket = need;
	}

	void needPrintReceivePacket(boolean need)
	{
	    m_needPrintReceivePacket = need;
	}
	
	void onReceivedData(byte[] bytes, int len)
	{
	    for(int i=0;i<len;i++)
	        handleOndByte(bytes[i] & 0xff);
	}
	
	void onReceivedData(byte[] bytes)
	{
	    for(byte b: bytes)
	        handleOndByte(b &0xff);
	}
	
	void handleOndByte(int b)
	{
	    if(m_status == FINDING_55)
	    {
	        // 看是否在找帧开始符，0x55。
	        if(b == 0x55)
	            m_status = NEED_AA;
	    }
	    else if(m_status == NEED_AA)
	    {
	        // 找到0x55后，下一个必须是0xAA，否则在次寻找0x55。
	        if(b == 0xAA)
	            m_status = NEED_LENGTH;
	        else
	            m_status = FINDING_55;
	    }
	    else if(m_status == NEED_LENGTH)
	    {
	        // 0xAA后接帧内容长度，检查长度是否小于DATA_LEN_MAX。
	        if(b < DATA_LEN_MAX)
	        {
	            m_frameLength = b;
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
	        m_buff[m_status] = (byte)b;
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
	
	
	void frameCompleted(byte[] frame, int len)
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
	        String str = "接收 <<";
	        for(int i=0;i<len;i++)
	            str += String.format(" %.2X",frame[i]&0xFF);
	        emit.printMessage(str);
	    }
	    //
	    
	    //DataInputStream ds = new DataInputStream(new ByteArrayInputStream(frame, 2, frame.length-2));
	    //float[] param = new float[(frame.length-2)/4];
	    //for(int i=0;i<param.length;i++){
	    //	param[i] = ds.readFloat();
	    //}
	    byte[] param = MyMath.toByte(frame, 2, len);
	    int param_len = len - 4;
	    //
	    switch((int)(frame[1]))
	    {
	    case Protocol.RETURN_ATTITUDE_QUATERNION :
	        {
	            if(param_len < 4*4)
	                break;
	            //
	            // 假设CPU为小端。
	            Quaternion atti = new Quaternion(MyMath.toFloat(param, 0, 4));
	            atti.nomalize();
	            emit.displayAttitudeQuaternion(atti);
	        }
	        break;
	    case Protocol.RETURN_TRANSLATION :
	        {
	            if(param_len < 3*4)
	                break;
	            //
	            // 假设CPU为小端。
	            float[] tran = MyMath.toFloat(param, 0, 3);
	            emit.displayTranslation(tran[0],tran[1],tran[2]);
	        }
	        break;
	    case Protocol.RETURN_ATTITUDE_ANGLE :
	        {
	            if(param_len < 3*4)
	                break;
	            //
	            float[] angle = MyMath.toFloat(param, 0, 3);
	            Quaternion atti = new Quaternion(angle[0],angle[1],angle[2]);
	            emit.displayAttitudeQuaternion(atti);
	        }
	        break;
	    case Protocol.RETURN_VECTOR :
	        {
	            if(param_len < 1+3*4)
	                break;
	            //
	            // 假设CPU为小端。
	            float[] xyz = MyMath.toFloat(param, 1, 3);;
	            emit.onReturnVector(param[0],xyz[0],xyz[1],xyz[2]);
	        }
	        break;
	    case Protocol.RETURN_STATUS :
	        {
	            if(param_len < 1+1)
	                break;
	            //
	            //QVector<uint8_t> v;
	            //for(int i=1;i<param_len;i++)
	            //    v.append(param[i]);
	            byte[] v = MyMath.toByte(param, 1);
	            emit.onReturnStatus(param[0],v);
	        }
	        break;
	    case Protocol.RETURN_MESSAGE :
	        {
	            if(param_len < 1)
	                break;
	            //
	            String str;
				try {
					byte[] t = new byte[param.length-2];
					System.arraycopy(param, 0, t, 0, param.length-2);
					str = new String(t, "UTF-8");
					// because utf8 is 3 byte, crc is 2 byte, there is need to sub before divide
					//str = str.substring(0, param.length/3); 
				} catch (UnsupportedEncodingException e) {
					str = "Can not convert to utf8";
					e.printStackTrace();
				}
	            emit.onReturnMessage( str);
	        }
	        break;
	    case Protocol.BOOTLOADER_STATUS :
	        {
	            if(param_len < 1)
	                break;
	            //
	            emit.onBootloaderStatus(param[0], MyMath.toByte(param, 1));
	        }
	        break;
	    case Protocol.PARAMETER :
	        {
	            if(param_len < 1)
	                break;
	            //
	            emit.onParameter(param);
	        }
	        break;
	    case Protocol.LOCK_ATTITUDE :
	        {
	            if(param_len < 1)
	                break;
	            emit.onControlLockAttitude(param);
	        }
	        break;
	    }
	}

	boolean checkFrameCrc(byte[] frame, int len)
	{
	    if(!m_checkCrc)
	        return true;
	    return MyMath.crc16(0,frame, len) == 0;
	}
	
	void transmitFrame(int protocol, int type, byte[] data, int len){
		transmitFrame((byte)protocol, (byte)type, data, len);
	}
	void transmitFrame(byte protocol, byte type, byte[] data, int len)
	{
	    byte[] frame = new byte[]{0x55,(byte)0xAA,(byte)(len+4),protocol,type};
	    byte[] b = new byte[frame.length + len + 2];
	    System.arraycopy(frame, 0, b, 0, frame.length);
	    System.arraycopy(data, 0, b, frame.length, len);
	    //b.append((char*)frame,sizeof(frame));
	    //b.append((char *)data,len);
	    int crc = MyMath.crc16(0,new byte[]{protocol},1);
	    crc = MyMath.crc16(crc,new byte[]{type},1);
	    crc = MyMath.crc16(crc,data,len);
	    //b.append((byte)(crc >> 8));
	    //b.append((byte)crc);
	    b[frame.length + len] = (byte)(crc >> 8);
	    b[frame.length + len + 1] = (byte)(crc);
	    //
	    // 判断是否输出包内容。
	    if(m_needPrintTransmitPacket)
	    {
	        String str = "发送 <<";
	        int print_len = b.length-3; // 3:去掉帧头，下同。
	        for(int i=0;i<print_len;i++)
	            str += String.format(" %.2X",b[3+i]&0xFF);
	        emit.printMessage(str);
	    }
	    //
	    emit.send(b);
	}
	
	
	public void cmd_getAttitudeQuaternion()
	{
	    transmitFrame(Protocol.VERSION,Protocol.GET_ATTITUDE_QUATERNION,null,0);
	}

	public void cmd_getTranslation()
	{
	    transmitFrame(Protocol.VERSION,Protocol.GET_TRANSLATION,null,0);
	}

	public void cmd_getAttitudeAngle()
	{
	    transmitFrame(Protocol.VERSION,Protocol.GET_ATTITUDE_ANGLE,null,0);
	}

	public void control_lockAttitude(byte[] param)
	{
	    transmitFrame(Protocol.VERSION,Protocol.LOCK_ATTITUDE,param,param.length);
	}

	public void control_setMode(int mode)
	{
	    transmitFrame(Protocol.VERSION,Protocol.SET_CONTROL_MODE,new byte[]{(byte)mode},1);
	}

	public void control_lockThrottle_setThrottle(float[] throttle)
	{
	    if(throttle.length != 4)
	        return;
	    byte[] t = MyMath.toByte(throttle);
	    transmitFrame(Protocol.VERSION,Protocol.LOCK_THROTTLE_SET_THROTTLE
	                  , t, t.length);
	}

	public void control_sendHeartbeat()
	{
	    transmitFrame(Protocol.VERSION,Protocol.HEARTBEAT_SEND,null,0);
	}

	public void cmd_getVector(byte type)
	{
	    transmitFrame(Protocol.VERSION,Protocol.GET_VECTOR,new byte[]{type},1);
	}

	public void cmd_getStatus(byte[] which)
	{
	    transmitFrame(Protocol.VERSION,Protocol.GET_STATUS,which,which.length);
	}

	public void cmd_bootloadCmd(byte cmd, byte[] param)
	{
	    byte[] t = new byte[param.length + 1];
	    t[0] = cmd;
	    System.arraycopy(param, 0, t, 1, param.length);
	    transmitFrame(Protocol.VERSION,Protocol.BOOTLOADER_CMD,t,t.length);
	}

	public void cmd_parameter(byte[] param)
	{
	    transmitFrame(Protocol.VERSION,Protocol.PARAMETER,param,param.length);
	}
	
	public void cmd_raw_control(short button, short x1, short y1, short x2, short y2){
		short[] b = new short[]{button,x1,y1,x2,y2};
		byte[] param = MyMath.toByte(b);
		transmitFrame(Protocol.VERSION,Protocol.RAW_CONTROL_DATA, param,param.length);	
	}
	public void cmd_raw_control(int button, int i, int j, int k, int l) {
		cmd_raw_control((short)button,(short)i,(short)j,(short)k,(short)l);
	}

	
	static class Signals{
		void displayAttitudeQuaternion(Quaternion atti){ Log.i(TAG, "displayAttitudeQuaternion ..."); }
	    void displayTranslation(float x,float y,float z){ Log.i(TAG, "displayTranslation ..."); }
	    void send(byte[] bytes){ Log.i(TAG, "send ..."); }
	    void onReturnVector(byte type,float x,float y,float z){ Log.i(TAG, "onReturnVector ..."); }
	    void onReturnStatus(byte which,byte[] value){ Log.i(TAG, "onReturnStatus ..."); }
	    void onReturnMessage(String msg){ Log.i(TAG, "onReturnMessage ..."); }
	    void onBootloaderStatus(byte status,byte[] param){ Log.i(TAG, "onBootloaderStatus ..."); }
	    void onParameter(byte[] param){ Log.i(TAG, "onParameter ..."); }
	    void onControlLockAttitude(byte[] param){ Log.i(TAG, "onControlLockAttitude ..."); }
	    void printMessage(String msg){ Log.i(TAG, "printMessage ..."); }
	}
}
