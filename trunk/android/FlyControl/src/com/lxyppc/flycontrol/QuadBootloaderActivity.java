package com.lxyppc.flycontrol;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import org.andengine.util.FileUtils;

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
	private Button mDownload;
	private Button mUpdate;
	private String mSDPath;
	private BootloaderSetting mSetting = new BootloaderSetting();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_PROGRESS);
		
		setContentView(R.layout.activity_quad_bootloader);
		mEditURL = (EditText)findViewById(R.id.editRemotePath);
		mEditPath = (EditText)findViewById(R.id.editLocalPath);
		
		mDownload = (Button)findViewById(R.id.quad_bootloader_download);
		mUpdate = (Button)findViewById(R.id.quad_bootloader_update);
		
		mSDPath = Environment.getExternalStorageDirectory().getPath();
		
		mInfo = (TextView)findViewById(R.id.textBootloaderInfo);
		
		mDownload.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				download();
			}
		});
		
		mUpdate.setOnClickListener(new View.OnClickListener(){
			public void onClick(View v) {
				update();
			}
		});
		
		set_setting();
	}
	
	protected void onDestroy(){
		get_setting();
		mSetting.save(this, MainActivity.PRE_NAME);
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
		public void run(){
			enable(false);
			OutputStream outs = null;
			InputStream ins = null;
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
					while((len = ins.read(buffer)) > 0){
						outs.write(buffer, 0, len);
						writen += len;
						setProgress(writen, length);
					}
					outs.flush();
				}
				setInfo(R.string.quad_bootloader_download_success);
			} catch (MalformedURLException e) {
				setInfo(e.toString());
				e.printStackTrace();
			} catch (IOException e) {
				setInfo(e.toString());
				e.printStackTrace();
			} finally {
				try {
					if(outs != null)outs.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
				try {
					if(ins != null)ins.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
				enable(true);
			}
		}
	}
	
	private void download(){
		new DownloadWorker().start();
	}
	
	private int boot_state = 0;
	private class UpdateWorker extends Thread{
		public void run(){
			enable(false);
			try{
				File file = null;
				file = new File(binPath());
				if(!file.exists()){
					setInfo(R.string.quad_bootloader_file_not_exsit);
					return;
				}
				setInfo(R.string.quad_bootloader_update_start);
			}finally{
			}
		}
	}
	
	private void update(){
		new UpdateWorker().start();
		
	}
	
	public void setInfo(int info){
		setInfo(getString(info));
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
			}
		});
	}
	
	private String binPath(){
		return mSDPath + "/" + mSetting.mPath;
	}
	
	public void set_setting(){
		mEditPath.setText(binPath());
		mEditURL.setText(mSetting.mURL);
	}
	
	public void get_setting(){
		mSetting.mPath = mEditPath.getText().toString();
		mSetting.mURL = mEditURL.getText().toString();
	}
	
	private static QuadBootloaderActivity instance = null;
	public static void setTitleProgress(float rate){
		int pos = (int)(rate*10000);
		if(instance!=null)instance.setProgress(pos);
	}
	
	
	
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
}
