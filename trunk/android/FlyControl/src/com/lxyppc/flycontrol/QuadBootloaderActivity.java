package com.lxyppc.flycontrol;

import com.lxyppc.flycontrol.bootloader.FileReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class QuadBootloaderActivity extends Activity {
	private EditText mEditURL;
	private EditText mEditPath;
	private TextView mInfo;
	private TextView mTextSDPath;
	private Button mDownload;
	private Button mUpdate;
	private Button mUpdateRemote;
	private Button mCancel;
	private String mSDPath;
	private BootloaderSetting mSetting = new BootloaderSetting();
	private FileReader mFileReader = new FileReader();
	private boolean mRemoteUpdate = false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_PROGRESS);
		
		setContentView(R.layout.activity_quad_bootloader);
		mEditURL = (EditText)findViewById(R.id.editRemotePath);
		mEditPath = (EditText)findViewById(R.id.editLocalPath);
		
		mDownload = (Button)findViewById(R.id.quad_bootloader_download);
		mUpdate = (Button)findViewById(R.id.quad_bootloader_update);
		mUpdateRemote = (Button)findViewById(R.id.quad_bootloader_update_url);
		mCancel = (Button)findViewById(R.id.quad_bootloader_cancel);
		
		mTextSDPath = (TextView)findViewById(R.id.textSDPath);
		mSDPath = Environment.getExternalStorageDirectory().getPath();
		
		mInfo = (TextView)findViewById(R.id.textBootloaderInfo);
		mTextSDPath.setText(mSDPath+"/");
		
		mDownload.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				get_setting();
				download();
			}
		});
		
		mUpdate.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				mRemoteUpdate = false;
				get_setting();
				update();
			}
		});
		
		mUpdateRemote.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				mRemoteUpdate = true;
				get_setting();
				update();
			}
		});
		
		mCancel.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				CleanDownloadWorker();
				CleanUpdateWorker();
			}
		});
		
		mSetting.restore(this, MainActivity.PRE_NAME);
		set_setting();
	}
	
	protected void onResume(){
		install_listener();
		super.onResume();
	}
	
	protected void onDestroy(){
		get_setting();
		mSetting.save(this, MainActivity.PRE_NAME);
		CleanDownloadWorker();
		CleanUpdateWorker();
		remove_listener();
		super.onDestroy();
	}
	
	private void check_dir(){
		String path = binPath();
		int pos2 = -1;
		pos2 = path.lastIndexOf("/");
		if(pos2>0){
			String pre_path = path.substring(0, pos2);
			Log.i("Quad", "Check path:"+pre_path);
			File dir = new File(pre_path);
			if(!dir.exists()){ 
				boolean r = dir.mkdirs();
				Log.i("Quad", "Make path " + r + ":" +pre_path);
			}
		}
	}
	
	private class DownloadWorker extends Thread{
		private InputStream ins = null;
		private boolean mRun = true;
		public void cancel(){
			if(ins != null){
				try {
					mRun = false;
					ins.close();
					this.join(500);
				} catch (IOException e) {
					e.printStackTrace();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		public void run(){
			enable(false);
			mRun = true;
			OutputStream outs = null;
			ins = null;
			File file = null;
			int length = 1000;
			int writen = 0;
			setProgress(writen, length);
			try {
				setInfo(R.string.quad_bootloader_download_start);
				URL url = new URL(mSetting.mURL);
				HttpURLConnection con = (HttpURLConnection)url.openConnection();
				con.connect();
				length = con.getContentLength();
				ins = con.getInputStream();
				check_dir();
				file = new File(binPath());
				if(file.exists()){
					file.delete();
				}
				if(file.createNewFile()){
					outs = new FileOutputStream(file);
					byte[] buffer = new byte[4*1024];
					int len = 0;
					while(mRun && (len = ins.read(buffer)) > 0){
						outs.write(buffer, 0, len);
						writen += len;
						if(mRun)setProgress(writen, length);
					}
					if(!mRun){
						setInfo(R.string.quad_bootloader_status_user_cancel_download);
					}else{
						outs.flush();
						setInfo(R.string.quad_bootloader_download_success);
					}
				}else{
					setInfo(R.string.quad_bootloader_file_create_fail);
				}
				
			} catch (MalformedURLException e) {
				if(!mRun){
					setInfo(R.string.quad_bootloader_status_user_cancel_download);
				}else{
					setInfo(e.toString());
				}
				//e.printStackTrace();
			} catch (IOException e) {
				if(!mRun){
					setInfo(R.string.quad_bootloader_status_user_cancel_download);
				}else{
					setInfo(e.toString());
				}
				//e.printStackTrace();
			} finally {
				try {
					if(outs != null)outs.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
				try {
					if(ins != null)ins.close();
					ins = null;
				} catch (IOException e) {
					e.printStackTrace();
				}
				setProgress(100,100);
				enable(true);
			}
		}
	}
	
	private DownloadWorker mDownloadWorker = null;
	private void CleanDownloadWorker(){
		if(mDownloadWorker != null){
			if(mDownloadWorker.isAlive()){
				mDownloadWorker.cancel();
			}
		}
	}
	private void download(){
		CleanDownloadWorker();
		mDownloadWorker = new DownloadWorker();
		mDownloadWorker.start();
	}
	
	private class UpdateWorker extends Thread{
		private InputStream ins = null;
		private boolean mRun = true;
		public void cancel(){
			try {
				if(ins != null){
					ins.close();
				}
				mRun = false;
				join(500);
			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		public void run(){
			ins = null;
			try{
				enable(false);
				int length = 0;
				if(!mRemoteUpdate){
					File file = null;
					file = new File(binPath());
					if(!file.exists()){
						setInfo(R.string.quad_bootloader_file_not_exsit);
						return;
					}
					setInfo(R.string.quad_bootloader_update_start);
					ins = (InputStream) new FileInputStream(file);
					length = (int)file.length();
				}else{
					setInfo(R.string.quad_bootloader_download_start);
					URL url = new URL(mSetting.mURL);
					HttpURLConnection con = (HttpURLConnection)url.openConnection();
					con.connect();
					length = con.getContentLength();
					ins = con.getInputStream();
				}
				log("File length is " + length);
				if(mFileReader.readFile(ins, length, mReaderProgress)){
					if(mRemoteUpdate){
						setInfo(R.string.quad_bootloader_download_success);
					}else{
						setInfo(R.string.quad_bootloader_update_load_file_success);
					}
				}else{
					setInfo(R.string.quad_bootloader_update_load_file_fail);
				}
				m_status = Status.ENTERING_BOOTLOADER; 
				while(mRun && onTimer()){
					Thread.sleep(20, 0);
				}
				
				if(!mRun){
					setInfo(R.string.quad_bootloader_status_user_cancel_update);
				}
				
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (MalformedURLException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}finally{
				try {
					if(ins != null)ins.close();
					ins = null;
				} catch (IOException e) {
					e.printStackTrace();
				}
				setProgress(100,100);
				enable(true);
			}
		}
	}
	
	private UpdateWorker mUpdateWorker = null;
	private void CleanUpdateWorker(){
		if(mUpdateWorker != null){
			if(mUpdateWorker.isAlive()){
				mUpdateWorker.cancel();
			}
		}
	}
	private void update(){
		CleanUpdateWorker();
		mUpdateWorker = new UpdateWorker();
		mUpdateWorker.start();
	}
	
	public void setInfo(int info){
		setInfo(getString(info));
	}
	
	private void cmd(int cmd, byte[] data){
		MainActivity.getParser().cmd_bootloadCmd((byte)cmd, data);
	}
	
	private int m_status = Status.INIT;
	private boolean onTimer(){
		boolean continue_run = true;
		
	    switch(m_status)
	    {
	    case Status.ENTERING_BOOTLOADER :
	        cmd(Protocol.BootloaderCmd.ENTER_BOOTLOADER,new byte[0]);
	        break;
	    case Status.BOOTLOADER_READY :
	        cmd(Protocol.BootloaderCmd.BEGIN_REFRESH_APP,new byte[0]);
	        break;
	    case Status.READY_TO_REFRESH_APP :
	        sendLengthCrc();
	        break;
	    case Status.WRITING_APP :
	        writeSomeBlock();
	        break;
	    case Status.REFRESH_APP_SUCCEED :
	        cmd(Protocol.BootloaderCmd.ENTER_APP,new byte[0]);
	        refreshComplete(true);
	        continue_run = false;
	        break;
	    case Status.REFRESH_APP_FAILED :
	        refreshComplete(false);
	        continue_run = false;
	        break;
	    case Status.RUNNING_APP :
	    	continue_run = false;
	        break;
	    }
	    return continue_run;
	}
	
	private void onStatus(int status,byte[] param) {
	    switch(status){
	    case Protocol.BootloaderStatus.STAY_AT_BOOTLOADER :
	        if(m_status != Status.ENTERING_BOOTLOADER)
	            break;
	        m_status = Status.BOOTLOADER_READY;
	        setInfo("已经进入Bootloader模式。");
	        break;
	    case Protocol.BootloaderStatus.READY_TO_REFRESH_APP :
	        if(m_status != Status.BOOTLOADER_READY)
	            break;
	        m_status = Status.READY_TO_REFRESH_APP;
	        setInfo("下位机已经准备好刷程序。");
	        break;
	    case Protocol.BootloaderStatus.WRITING_APP :
	        if(m_status == Status.READY_TO_REFRESH_APP)
	        {
	            // 准备写。
	            m_status = Status.WRITING_APP;
	            setInfo("开始写入程序。");
	        }
	        else if(m_status == Status.WRITING_APP)
	        {
	            // 正在写。
	            onWriting(param);
	        }
	        break;
	    case Protocol.BootloaderStatus.BLOCK_REMAIN :
	        if(m_status != Status.WRITING_APP)
	            break;
	        onRemainRegion(param);
	        break;
	    case Protocol.BootloaderStatus.WRITE_APP_SUCCEED :
	        if(m_status != Status.WRITING_APP)
	            break;
	        m_status = Status.REFRESH_APP_SUCCEED;
	        setInfo("程序更新完毕，即将运行新程序");
	        cmd(Protocol.BootloaderCmd.ENTER_APP, new byte[0]);
	        setProgress(100,100);
	        //ui->progressBar->setValue(ui->progressBar->maximum());
	        break;
	    case Protocol.BootloaderStatus.APP_CRC_ERROR :
	        if(m_status != Status.WRITING_APP)
	            break;
	        m_status = Status.REFRESH_APP_FAILED;
	        setInfo("程序更新失败。请重新尝试。");
	        setProgress(100,100);
	        //ui->progressBar->setValue(ui->progressBar->maximum());
	        break;
	    }
	}
	
	
	int sendLengthCrc()
	{
	    int len = 0;
	    int crc = 0;
	    len = mFileReader.getLength();
	    crc = mFileReader.getCrc();
	    if(crc == 0)
	        return -1;
	    //
	    //QByteArray ba;
	    //for(int i=0;i<4;i++)
	        //ba.append(((char *)&len)[i]);
	    //for(int i=0;i<4;i++)
	        //ba.append(((char *)&crc)[i]);
	    cmd(Protocol.BootloaderCmd.APP_LENGTH_CRC,MyMath.toByte(new int[]{len,crc}));
	    //
	    return 0;
	}
	
	int wait_times = 0;
	void writeSomeBlock()
	{
	    if(wait_times > 0)
	    {
	        wait_times --;
	        return;
	    }
	    //
	    byte[] send = null;
	    for(int i=0;i<6;i++)
	    {
	    	send = mFileReader.getNextBlocks(26);
	        if(send == null)
	        {
	            cmd(Protocol.BootloaderCmd.QUEST_REMAIN_BLOCK, new byte[0]);
	            wait_times = 5;
	            break;
	        }
	        else
	        {
	            cmd(Protocol.BootloaderCmd.WRITE_APP,send);
	        }
	    }
	}
	
	void refreshComplete(boolean isSucceed)
	{
	    m_status = Status.INIT;
	    //ui->btStartRefreshApp->setText(QString::fromUtf8("更新程序"));
	    //ui->btSelectFile->setEnabled(true);
	    //ui->leFilePath->setEnabled(true);
	    //m_timer.stop();
	}
	
	int onWriting(byte[] param)
	{
	    if(param.length < 4)
	        return -1;
	    //
	    int blocksWritten = MyMath.toInt(param);
	    //for(int i=0;i<4;i++)
	    //    ((uint8_t *)&blocksWritten)[i] = param[i];
	    //
	    //int value = ui->progressBar->maximum() * blocksWritten / m_fileReader.getBlocksAmount();
	    //ui->progressBar->setValue(value);
	    if(mUpdateWorker != null && mUpdateWorker.isAlive()){
	    	setProgress(blocksWritten, mFileReader.getBlocksAmount());
	    }
	    //
	    return 0;
	}

	int onRemainRegion(byte[] raw)
	{
	    int len = raw.length / 4;
	    if(len == 0 && mFileReader.isNoMoreblock())
	        return -1;
	    //
	    for(int i=0;i<len;i++)
	    {
	    	short r[] = MyMath.toShort(raw, i*4, 2);
	        int index = r[0];
	        int length = r[1];
	        //((uint8_t *)&index)[0] = raw[i*4 + 0];
	        //((uint8_t *)&index)[1] = raw[i*4 + 1];
	        //((uint8_t *)&length)[0] = raw[i*4 + 2];
	        //((uint8_t *)&length)[1] = raw[i*4 + 3];
	        //
	        mFileReader.addRegion(index,length);
	    }
	    //
	    return 0;
	}
	
	
	private String mInfoText;
	public void setInfo(String info){
		mInfoText = getString(R.string.quad_com_status) + info;
		runOnUiThread(new Runnable(){
			public void run() {
				mInfo.setText(mInfoText);
			}
		});
	}
	
	private FileReader.Progress mReaderProgress = new FileReader.Progress() {
		public void setProgress(int pos, int total) {
			QuadBootloaderActivity.this.setProgress(pos, total);
		}
	};
	private float mProgressPos = 0f;
	private void setProgress(int pos, int total){
		if(total == 0){
			mProgressPos = 0f;
		}else{
			mProgressPos = (float)pos/(float)total;
		}
		runOnUiThread(new Runnable(){
			public void run() {
				setProgress((int)(10000*mProgressPos));
			}
		});
	}
	
	private boolean mEnable = false;
	private void enable(boolean enable){
		mEnable = enable;
		runOnUiThread(new Runnable(){
			public void run() {
				mDownload.setEnabled(mEnable);
				mUpdate.setEnabled(mEnable);
				mUpdateRemote.setEnabled(mEnable);
			}
		});
	}
	
	private String binPath(){
		return mSDPath + "/" + mSetting.mPath;
	}
	
	public void set_setting(){
		mEditPath.setText(mSetting.mPath);
		mEditURL.setText(mSetting.mURL);
	}
	
	public void get_setting(){
		mSetting.mPath = mEditPath.getText().toString();
		mSetting.mURL = mEditURL.getText().toString();
	}
	
	private MainActivity.OnReturnMessage onReturnMessage = new MainActivity.OnReturnMessage(){
		public void onReturnMessage(String param) {
			setInfo(param);
		}
	};
	private MainActivity.OnBootloaderStatus onBootloaderStatus = new MainActivity.OnBootloaderStatus(){
		public void onBootloaderStatus(byte status,byte[] param) {
			onStatus(status<0?status+256:status, param);
		}
	};
	public void install_listener(){
		MainActivity.setOnBootloaderStatusListener(onBootloaderStatus);
		MainActivity.setOnReturnMessageListener(onReturnMessage);
	}
	public void remove_listener(){
		MainActivity.setOnParameterListener(null);
		MainActivity.setOnReturnMessageListener(null);
	}
	
	
	private static final class Status{
        final static int INIT                   = 1;
        final static int ENTERING_BOOTLOADER    = 2;
        final static int BOOTLOADER_READY       = 3;
        final static int READY_TO_REFRESH_APP   = 4;
        final static int WRITING_APP            = 5;
        //const static int WAITING_APP_FINISH     = 6;
        final static int REFRESH_APP_SUCCEED    = 7;
        final static int REFRESH_APP_FAILED     = 8;
        final static int RUNNING_APP            = 9;
    };
	
	public static class BootloaderSetting{
		private static final String DEF_APP_URL = "http://etootle-flight.googlecode.com/svn/trunk/release/quad_app.bin";
		private static final String DEF_APP_PATH = "flycontrol/app.bin";
		public String mURL = DEF_APP_URL;
		public String mPath = DEF_APP_PATH;
		BootloaderSetting(){}

		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putString("quad_bootloader_url", mURL);
			editor.putString("quad_bootloader_path", mPath);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mURL = cfg.getString("quad_bootloader_url", DEF_APP_URL);
			mPath = cfg.getString("quad_bootloader_path", DEF_APP_PATH);
		}
	}
	
	static public void log(String msg){
		Log.i("Quad", msg);
	}
}
