package com.lxyppc.flycontrol;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.TabActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TabHost.TabSpec;

public class QuadParamSettingActivity extends TabActivity {
	private static final String SENSOR_TAB = "sensor_tab";
	private static final String PID_TAB = "pid_tab";
	private static final String HORIZON_TAB = "horizon_tab";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_PROGRESS);
		instance = this;
		
		setContentView(R.layout.activity_quad_param_setting);
		TabHost host = getTabHost();
		Intent intent = new Intent().setClass(this, SensorSettingActivity.class);
		TabSpec spec = host.newTabSpec(SENSOR_TAB);
		spec.setIndicator(getString(R.string.quad_sensor));
		spec.setContent(intent);
		host.addTab(spec);
		
		intent = new Intent().setClass(this, PIDSettingActivity.class);
		spec = host.newTabSpec(PID_TAB);
		spec.setIndicator(getString(R.string.quad_pid));
		spec.setContent(intent);
		host.addTab(spec);
		
		intent = new Intent().setClass(this, HorizonSettingActivity.class);
		spec = host.newTabSpec(HORIZON_TAB);
		spec.setIndicator(getString(R.string.quad_horizon));
		spec.setContent(intent);
		host.addTab(spec);
		final int height = 45;
		host.getTabWidget().getChildAt(0).getLayoutParams().height = height;
		host.getTabWidget().getChildAt(1).getLayoutParams().height = height;
		host.getTabWidget().getChildAt(2).getLayoutParams().height = height;
	}
	
	protected void onDestroy(){
		super.onDestroy();
	}
	
	private static QuadParamSettingActivity instance = null;
	
	public static void setTitleProgress(float rate){
		int pos = (int)(rate*10000);
		if(instance!=null)instance.setProgress(pos);
	} 
	
	// class for the param button
	public static class ParamButton{
		private static int TIMEOUT_MS = 2000; // 2 seconds
		ParamButton(Context ctx, int startPos, int count, EditText[] edits){
			mCtx = ctx;
			mStart = startPos;
			mCount = count;
			mEdits = edits;
			getRam = new Button(mCtx);
			getRam.setText(R.string.quad_get_ram);
			setRam = new Button(mCtx);
			setRam.setText(R.string.quad_set_ram);
			
			getFlash = new Button(mCtx);
			getFlash.setText(R.string.quad_get_flash);
			setFlash = new Button(mCtx);
			setFlash.setText(R.string.quad_set_flash);
			resetFlash = new Button(mCtx);
			resetFlash.setText(R.string.quad_reset_flash);
			
			info = new TextView(mCtx);
			
			getRam.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { clicked(Protocol.Parameter.GET_RAM); }});
			setRam.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { clicked(Protocol.Parameter.SET_RAM); }});
			getFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { clicked(Protocol.Parameter.GET_FLASH); }});
			setFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { clicked(Protocol.Parameter.WRITE_FLASH); }});
			resetFlash.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) { clicked(Protocol.Parameter.RESET_FLASH); }});
			
		}
		
		void append2layout(LinearLayout tl){
			LinearLayout ll = new LinearLayout(mCtx);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			ll.addView(getRam,lp);
			ll.addView(setRam,lp);
			tl.addView(info);
			tl.addView(ll);
			
			ll = new LinearLayout(mCtx);
			ll.addView(getFlash,lp);
			ll.addView(setFlash,lp);
			ll.addView(resetFlash,lp);
			tl.addView(ll);
		}
		private MainActivity.OnParameter onParam = new MainActivity.OnParameter(){
			public void onParameter(byte[] param) {
				System.arraycopy(param, 0, returnParam, 0, param.length);
				synchronized(mSync){
					m_return = true;
					mSync.notify();
				}
			}
		};
		private MainActivity.OnReturnMessage onReturnMessage = new MainActivity.OnReturnMessage(){
			public void onReturnMessage(String param) {
				info.setText(param);
			}
		};
		
		private int workMode = 0;
		private boolean m_return = false;
		private byte[] returnParam = new byte[64];
		private Object mSync = new Object();
		boolean waitResult(){
			//Log.i("Quad", "start wait");
			try {
				synchronized(mSync){
					mSync.wait(TIMEOUT_MS);
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
				setMessage(e.toString());
				//Log.i("Quad", "start wait fail");
				return false;
			}
			//Log.i("Quad", "start wait success");
			if(!m_return){
				setMessage(mCtx.getString(R.string.quad_com_timeout));
			}
			return m_return;
		}
		private class Worker extends Thread{
			public void run() {
				enable(false);
				setProgress(0);
				switch(workMode){
					case Protocol.Parameter.GET_RAM:
					case Protocol.Parameter.GET_FLASH:
					{
						int idx;
						for(idx=0;idx<mCount;idx++){
							byte[] param = new byte[]{(byte) workMode,(byte) (mStart+idx), 1};
							sendParam(param);
							if(!waitResult()){
								break;
							}
							int mode = returnParam[0];
							if(mode<0)mode+=256;
							if(mode == workMode+1){
								int pos = returnParam[1]<0?returnParam[1]+256:returnParam[1];
								pos-=mStart;
								if(idx<mEdits.length && idx == pos){
									float r[] = MyMath.toFloat(returnParam, 3, 1);
									setText(idx, String.format("%f",r[0]));
								}
								setProgress(idx+1);
							}else{
								break;
							}
						}
						if(idx == mCount){
							setMessage(mCtx.getString(R.string.quad_com_sucess));
						}
					}
					break;
					case Protocol.Parameter.SET_RAM:
					{
						int idx;
						for(idx=0;idx<mCount;idx++){
							byte[] param = new byte[]{(byte) workMode,(byte) (mStart+idx), 1, 0, 0, 0, 0};
							if(idx<mEdits.length){
								String r = mEdits[idx].getText().toString();
								
								float[] f = new float[]{0.0f};
								try{
									f[0] = Float.parseFloat(r);
								}catch(NumberFormatException e){
									
								}
								byte[] b = MyMath.toByte(f);
								System.arraycopy(b, 0, param, 3, 4);
								sendParam(param);
								if(!waitResult()){
									break;
								}
								int mode = returnParam[0];
								if(mode<0)mode+=256;
								if(mode == workMode -1){
									float r2[] = MyMath.toFloat(returnParam, 3, 1);
									setText(idx, String.format("%f",r2[0]));
								}else{
									setMessage(mCtx.getString(R.string.quad_com_data_error));
									break;
								}
							}
							setProgress(idx+1);
						}
						if(idx == mCount){
							setMessage(mCtx.getString(R.string.quad_com_sucess));
						}
					}
					break;
					case Protocol.Parameter.WRITE_FLASH:
					case Protocol.Parameter.RESET_FLASH:	
					{
						byte[] param = new byte[]{(byte) workMode};
						sendParam(param);
						setProgress(mCount);
					}
					break;
				}
				enable(true);
			}
		};
		
		private void sendParam(byte[] param){
			m_return = false;
			MainActivity.getParser().cmd_parameter(param);
		}
		
		public void install_listener(){
			MainActivity.setOnParameterListener(onParam);
			MainActivity.setOnReturnMessageListener(onReturnMessage);
		}
		public void remove_listener(){
			MainActivity.setOnParameterListener(null);
			MainActivity.setOnReturnMessageListener(null);
		}
		
		static int x = 0;
		private void clicked(int id){
			workMode = id;
			if(id == Protocol.Parameter.WRITE_FLASH){
				Dialog alertDialog = new AlertDialog.Builder(mCtx)
				.setTitle(R.string.app_name)
				.setMessage(R.string.quad_confirm_write_flash)
				.setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener(){
					public void onClick(DialogInterface arg0, int arg1) {
						new Worker().start();
					}})
				.setNegativeButton(android.R.string.no, new DialogInterface.OnClickListener(){
					public void onClick(DialogInterface arg0, int arg1) {}})
				.create();
				alertDialog.show();
			}else if(id == Protocol.Parameter.RESET_FLASH){
				Dialog alertDialog = new AlertDialog.Builder(mCtx)
				.setTitle(R.string.app_name)
				.setMessage(R.string.quad_confirm_reset_flash)
				.setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener(){
					public void onClick(DialogInterface arg0, int arg1) {
						new Worker().start();
					}})
				.setNegativeButton(android.R.string.no, new DialogInterface.OnClickListener(){
					public void onClick(DialogInterface arg0, int arg1) {}})
				.create();
				alertDialog.show();
			}else{
				new Worker().start();
			}
		}
		
		private void setText(int idx, String text){
			class MyRun implements Runnable{
				MyRun(int i, String s){
					mIdx = i;
					mStr = s;
				}
				private int mIdx;
				private String mStr;
				public void run() {
					mEdits[mIdx].setText(mStr);
				}
			}
			((Activity) mCtx).runOnUiThread(new MyRun(idx,text));
		}
		
		private int mProgressPos = 0;
		private void setProgress(int pos){
			mProgressPos = pos;
			((Activity) mCtx).runOnUiThread(new Runnable(){
				public void run() {
					QuadParamSettingActivity.setTitleProgress((float)mProgressPos/(float)mCount);
				}
			});
		}
		
		private String mMessage = null;
		private void setMessage(String msg){
			mMessage = mCtx.getString(R.string.quad_com_status) + msg;
			((Activity) mCtx).runOnUiThread(new Runnable(){
				public void run() {
					info.setText(mMessage);
				}
			});
		}
		
		private boolean mEnable = false;
		private void enable(boolean enable){
			mEnable = enable;
			((Activity) mCtx).runOnUiThread(new Runnable(){
				public void run() {
					getRam.setEnabled(mEnable);
					setRam.setEnabled(mEnable);
					getFlash.setEnabled(mEnable);
					setFlash.setEnabled(mEnable);
					resetFlash.setEnabled(mEnable);
				}
			});
			
		}
		private Context mCtx;
		private Button getRam;
		private Button setRam;
		private Button getFlash;
		private Button setFlash;
		private Button resetFlash;
		private TextView info;
		private EditText[] mEdits;
		private int mStart;
		private int mCount;
	}
}
