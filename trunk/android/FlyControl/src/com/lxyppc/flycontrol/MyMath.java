//     Copyright (c) 2013 js200300953@qq.com,lxyppc@163.com All rights reserved.
//         ========圆点博士微型四轴飞行器配套程序声明========
// 
// 圆点博士微型四轴飞行器配套程序包括上位机程序、下位机Bootloader和
//     下位机App，及它们的源代码，以下总称“程序”。
// 程序由js200300953,lxyppc编写。
// 程序仅为使用者提供参考，js200300953,lxyppc不对程序提供任何明示或暗含的担保，
//     不对在使用该程序中出现的意外或者损失负责，
//     也不对因使用该程序而引起的第三方索赔负责。
// 使用者可以以学习为目的修改和使用该程序，请勿以商业的目的使用该程序。
// 修改该程序时，必须保留原版权声明，并且不能修改原版权声明。
// 
// 更多资料见：
//     http://blog.sina.com.cn/js200300953
//     http://hi.baidu.com/lxyppc
//     http://www.etootle.com/
//     http://www.amobbs.com/thread-5504090-1-1.html
//     圆点博士微型四轴飞行器QQ群：276721324

package com.lxyppc.flycontrol;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import android.util.Log;

public class MyMath {
	public static int crc16(int crc, byte[] data, int len){
		final int[] crc_tab = new int[]
			    {
			        0x0000 , 0x1021 , 0x2042 , 0x3063 , 0x4084 , 0x50A5 , 0x60C6 , 0x70E7 ,
			        0x8108 , 0x9129 , 0xA14A , 0xB16B , 0xC18C , 0xD1AD , 0xE1CE , 0xF1EF
			    };
		int h_crc;
	    for(int i=0;i<len;i++){
	    	byte b = data[i];
	    	h_crc = (crc>>12) & 0xf;
	    	crc <<= 4;
	    	crc ^= crc_tab[ (h_crc ^ (b>>4)) &0x0f ];
	    	crc &= 0xffff;
	    	h_crc = (crc>>12) & 0xf;
	    	crc <<= 4;
	    	crc ^= crc_tab[ h_crc ^ (b&0x0f) ];
	    	crc &= 0xffff;
	    }
	    return crc;
	}
	public static int crc16(int crc, byte[] data){
		return crc16(crc,data,data.length);
	}
	
	public static float toFloat(byte[] data){
		return toFloat(data,0);
	}
	public static float toFloat(byte[] data, int offset){
		if(data.length-offset>3){
			return toFloat(data, offset, 1)[0];
		}
		return 0f;
	}
	public static float[] toFloat(byte[] data, int offset, int count){
		int l = (data.length - offset) / 4;
		if(count>l)count = l;
		float[] r = new float[count];
		byte[] temp = new byte[data.length - offset];
		System.arraycopy(data, offset, temp, 0, temp.length);
		ByteBuffer byteBuf = ByteBuffer.wrap(temp);
		byteBuf = byteBuf.order(null);
		for(int i=0;i<count;i++){
			r[i] = byteBuf.getFloat(i*4);
		}
		//DataInputStream ds = new DataInputStream(new ByteArrayInputStream(data, offset, data.length-offset));
		//for(int i=0;i<count;i++){
	    //	try {
				//r[i] = ds.readFloat();
		//	} catch (IOException e) {
		//		break;
		//	}
	    //}
		return r;
	}
	
	public static int toInt(byte[] data){
		return toInt(data,0);
	}
	public static int toInt(byte[] data, int offset){
		if(data.length-offset>3){
			return toInt(data, offset, 1)[0];
		}
		return 0;
	}
	public static int[] toInt(byte[] data, int offset, int count){
		int l = (data.length - offset) / 4;
		if(count>l)count = l;
		int[] r = new int[count];
		byte[] temp = new byte[data.length - offset];
		System.arraycopy(data, offset, temp, 0, temp.length);
		ByteBuffer byteBuf = ByteBuffer.wrap(temp);
		byteBuf = byteBuf.order(null);
		for(int i=0;i<count;i++){
			r[i] = byteBuf.getInt(i*4);
		}
		return r;
	}
	
	public static short toShort(byte[] data){
		return toShort(data,0);
	}
	public static short toShort(byte[] data, int offset){
		if(data.length-offset>3){
			return toShort(data, offset, 1)[0];
		}
		return 0;
	}
	public static short[] toShort(byte[] data, int offset, int count){
		int l = (data.length - offset) / 2;
		if(count>l)count = l;
		short[] r = new short[count];
		byte[] temp = new byte[data.length - offset];
		System.arraycopy(data, offset, temp, 0, temp.length);
		ByteBuffer byteBuf = ByteBuffer.wrap(temp);
		byteBuf = byteBuf.order(null);
		for(int i=0;i<count;i++){
			r[i] = byteBuf.getShort(i*2);
		}
		return r;
	}
	
	public static byte[] toByte(byte[] data, int offset){
		return toByte(data, offset, data.length);
	}
	
	public static byte[] toByte(byte[] data, int offset, int length){
		if(length > data.length)length = data.length;
		byte[] r = new byte[length - offset];
		System.arraycopy(data, offset, r, 0, r.length);
		return r;
	}
	
	public static byte[] toByte(float[] data){
		byte byteArray[] = new byte[data.length*4];  
		ByteBuffer byteBuf = ByteBuffer.wrap(byteArray); 
		byteBuf = byteBuf.order(null); 
		FloatBuffer floatBuf = byteBuf.asFloatBuffer();  
		floatBuf.put (data);
		return byteArray; 
	}
	
	public static byte[] toByte(short[] data){
		byte byteArray[] = new byte[data.length*2];  
		ByteBuffer byteBuf = ByteBuffer.wrap(byteArray); 
		byteBuf = byteBuf.order(null); 
		ShortBuffer shortBuf = byteBuf.asShortBuffer();
		shortBuf.put (data);
		return byteArray; 
	}
	
	public static byte[] toByte(int[] data){
		byte byteArray[] = new byte[data.length*4];  
		ByteBuffer byteBuf = ByteBuffer.wrap(byteArray); 
		byteBuf = byteBuf.order(null); 
		IntBuffer intBuf = byteBuf.asIntBuffer();
		intBuf.put (data);
		return byteArray; 
	}
	
	public static void showBytes(byte[] data, String desc){
		String t = "";
		for(byte b:data){
			t += String.format("%x,", b<0?b+256:b);
		}
		Log.i("FLYC", desc + ": " + t);
	}
	
	public static void showBytes(float[] data, String desc){
		String t = "";
		for(float b:data){
			t += String.format("%f,", b);
		}
		Log.i("FLYC", desc + ": " + t);
	}
	
	public static int toInt(String str) {
		return toInt(str,0);
	}
	
	public static int toInt(String str, int def) {
		int r;
		try{
			r = Integer.parseInt(str);
		} catch(NumberFormatException e){
			r = def;
		}
		return r;
	}
}
