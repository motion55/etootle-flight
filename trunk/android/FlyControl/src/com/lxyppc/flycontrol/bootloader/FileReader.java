package com.lxyppc.flycontrol.bootloader;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.Queue;

import com.lxyppc.flycontrol.MyMath;

public class FileReader {
	private static final int BYTE_PER_BLOCK = 4;
	
	private byte[] m_fileContent;
	private Region m_currentRegion = new Region();
	private Queue<Region> m_regionsNotWrite = new LinkedList<Region>();
	private int m_blocksAmount;
	
	public FileReader(){
	}
	
	
	public boolean readFile(InputStream fileIns, int length, Progress progress){
		ByteArrayOutputStream outs = new ByteArrayOutputStream(length);
		byte[] buffer = new byte[1024*4];
		int r = 0;
		int wr = 0;
		boolean ret = false;
		try {
			while( (r = fileIns.read(buffer))>0 ){
				outs.write(buffer, 0, r);
				wr += r;
				if(progress != null)progress.setProgress(wr, length);
			}
			if((wr%4) != 0){
				byte[] left = new byte[wr%4];
				outs.write(left);
			}
			outs.close();
			m_fileContent = outs.toByteArray();
			ret = true;
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		m_blocksAmount = m_fileContent.length / 4;
		m_currentRegion.index = 0;
	    m_currentRegion.length = m_blocksAmount;
	    m_regionsNotWrite.clear();
		return ret;
	}
	
	public byte[] getNextBlocks(int maxSize)
	{
		;
	    // 保证m_currentRegion有效。
	    if(m_currentRegion.length <= 0)
	    {
	        if(m_regionsNotWrite.isEmpty())
	            return null;
	        m_currentRegion = m_regionsNotWrite.poll();
	    }
	    //
	    // 求实际的长度。
	    int len = (maxSize-2)/BYTE_PER_BLOCK;
	    if(len > m_currentRegion.length)
	        len = m_currentRegion.length;
	    //
	    // 复制数据，推进指针。
	    byte[] blocks = new byte[len*BYTE_PER_BLOCK + 2];
	    short t = (short)m_currentRegion.index;
	   // blocks->clear();
	    byte[] ts = MyMath.toByte(new short[]{t});
	    blocks[0] = ts[0];
	    blocks[1] = ts[1];
	    //blocks->append(m_fileContent.mid(m_currentRegion.index*BYTE_PER_BLOCK,len*BYTE_PER_BLOCK));
	    System.arraycopy(m_fileContent, m_currentRegion.index*BYTE_PER_BLOCK, blocks, 2, len*BYTE_PER_BLOCK);
	    m_currentRegion.index  += len;
	    m_currentRegion.length -= len;
	    //
	    return blocks;
	}
	
	public int getLength(){ return m_blocksAmount; }
	
	public int getCrc(){
	    if(m_fileContent.length <= 0)
	        return 0;
	    return MyMath.crc16(0, m_fileContent);
	}
	
	public int addRegion(int index, int length)
	{
	    if(index < 0 || length <= 0 || index+length > m_blocksAmount)
	        return -1;
	    //
	    Region r = new Region();
	    r.index = index;
	    r.length = length;
	    //
	    m_regionsNotWrite.add(r);
	    //
	    return 0;
	}
	
	public int getBlocksAmount()
	{
	    return m_blocksAmount;
	}

	public boolean isNoMoreblock()
	{
	    return ((m_currentRegion.length == 0) && (m_regionsNotWrite.isEmpty()));
	}
	
	private class Region{
		 Region() { index = length = 0; }
        //
        int index;
        int length;
	}
	
	public static interface Progress{
		void setProgress(int pos, int total);
	}
}
