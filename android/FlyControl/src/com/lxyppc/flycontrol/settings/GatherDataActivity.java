package com.lxyppc.flycontrol.settings;

import java.util.ArrayList;
import java.util.List;

import com.lxyppc.flycontrol.MainActivity;
import com.lxyppc.flycontrol.Protocol;
import com.lxyppc.flycontrol.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.text.InputType;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TextView;

public class GatherDataActivity extends Activity {
	private static int TIMEOUT_MS = 2000;
	private QuadParamSettingActivity.ParamEdit params;
	private Button mDataType;
	private Button mSampleOnce;
	private Button mSampleMore;
	private EditText mSampleTime;
	private EditText mSampleInterval;
	private GatherSetting mGatherSetting = new GatherSetting();
	private TextView info;
	private boolean mSampeMultiple = false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_gather_setting);
		
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.gather_data_layout);
		
		params = new QuadParamSettingActivity.ParamEdit(this, 1, 3);
		params.append2layout(tl, R.array.quad_gather_data_head, R.array.quad_gather_data_names);
		
		LinearLayout ll = new LinearLayout(this);
		LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
		lp.weight = 2;
		
		TextView v = new TextView(this);
		v.setText(R.string.quad_gather_data_source);
		mDataType = new Button(this);
		
		ll.addView(v,lp);
		ll.addView(mDataType,lp);
		tl.addView(ll);
		
		ll = new LinearLayout(this);
		v = new TextView(this);
		v.setText(R.string.quad_gather_sample_count);
		mSampleTime = new EditText(this);
		mSampleTime.setInputType(InputType.TYPE_CLASS_NUMBER);
		ll.addView(v,lp);
		ll.addView(mSampleTime,lp);
		//tl.addView(ll);
		
		//ll = new LinearLayout(this);
		v = new TextView(this);
		v.setText(R.string.quad_gather_sample_interval);
		mSampleInterval = new EditText(this);
		mSampleInterval.setInputType(InputType.TYPE_CLASS_NUMBER);
		ll.addView(v,lp);
		ll.addView(mSampleInterval,lp);
		tl.addView(ll);
		
		ll = new LinearLayout(this);
		info = new TextView(this);
		ll.addView(info,lp);
		tl.addView(ll);
		
		ll = new LinearLayout(this);
		mSampleOnce = new Button(this);
		mSampleOnce.setText(R.string.quad_gather_sample_once);
		mSampleMore = new Button(this);
		mSampleMore.setText(R.string.quad_gather_sample_more);
		ll.addView(mSampleOnce,lp);
		ll.addView(mSampleMore,lp);
		tl.addView(ll);
		
		mGatherSetting.restore(this, MainActivity.PRE_NAME);
		update_setting();
		
		mDataType.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				Dialog alertDialog = new AlertDialog.Builder(GatherDataActivity.this)
				.setTitle(R.string.quad_gather_select_type)
				.setItems(R.array.quad_gather_data_types, new DialogInterface.OnClickListener(){
					@Override
					public void onClick(DialogInterface dialog, int which) {
						mGatherSetting.mSampleType = which + 1;
						update_setting();
					}
					
				})
				.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener(){
					public void onClick(DialogInterface dialog, int which) {}
				})
				.create()
				;
				alertDialog.show();
			}
			
		});
		mSampleOnce.setOnClickListener(new View.OnClickListener(){
			public void onClick(View arg0) {
				get_setting();
				mSampeMultiple = false;
				new Worker().start();
			}
		});
		mSampleMore.setOnClickListener(new View.OnClickListener(){
			public void onClick(View arg0) {
				get_setting();
				mSampeMultiple = true;
				new Worker().start();
			}
		});
	}
	
	public void update_setting(){
		String[] r = this.getResources().getStringArray(R.array.quad_gather_data_types);
		if(mGatherSetting.mSampleType>0 && mGatherSetting.mSampleType<=r.length){
			mDataType.setText(">> " + r[mGatherSetting.mSampleType-1]);
		}else{
			mDataType.setText(">> " + getString(R.string.quad_gather_unknown_data));
		}
		mSampleTime.setText(String.format("%d", mGatherSetting.mSampleCount));
		mSampleInterval.setText(String.format("%d", mGatherSetting.mSampleInterval));
	}
	
	public void get_setting(){
		try{
			mGatherSetting.mSampleCount = Integer.parseInt(mSampleTime.getText().toString());
		}catch(NumberFormatException e){
			mGatherSetting.mSampleCount = GatherSetting.DEF_COUNT;
		}
		try{
			mGatherSetting.mSampleInterval = Integer.parseInt(mSampleInterval.getText().toString());
		}catch(NumberFormatException e){
			mGatherSetting.mSampleInterval = GatherSetting.DEF_INTERVAL;
		}
	}
	
	public boolean onContextItemSelected(MenuItem item) {
		params.onContextItemSelected(item);
		return super.onContextItemSelected(item);
	}
	
	
	private Object mSync = new Object();
	private boolean mReturn = false;
	private class Vector{
		Vector(float x, float y, float z){
			this.x = x; this.y = y; this.z = z;
		}
		float x;
		float y;
		float z;
	}
	private List<Vector> result = new ArrayList<Vector>();
	private MainActivity.OnReturnVector onReturnVector = new MainActivity.OnReturnVector(){
		@Override
		public void onReturnVector(byte type, float x, float y, float z) {
			synchronized(mSync){
				result.add(new Vector(x,y,z));
				mReturn = true;
				mSync.notify();
			}
		}
	};
	private MainActivity.OnReturnMessage onReturnMessage = new MainActivity.OnReturnMessage(){
		public void onReturnMessage(String param) {
			setMessage(param);
		}
	};
	
	boolean waitResult(){
		//Log.i("Quad", "start wait");
		try {
			synchronized(mSync){
				mSync.wait(TIMEOUT_MS);
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
			setMessage(e.toString());
			return false;
		}
		if(!mReturn){
			setMessage(getString(R.string.quad_com_timeout));
		}
		return mReturn;
	}
	private class Worker extends Thread{
		public void run() {
			enable(false);
			int count = 1;
			if(mSampeMultiple){
				count = mGatherSetting.mSampleCount;
			}
			setProgress(0, count);
			setMessage(getString(R.string.quad_com_start));
			result.clear();
			int idx;
			for(idx=0;idx<count;idx++){
				long x = System.currentTimeMillis();
				getVector();
				if(!waitResult()){
					break;
				}
				x = System.currentTimeMillis() - x;
				if(mGatherSetting.mSampleInterval > x){
					long s = mGatherSetting.mSampleInterval - x;
					try {
						Thread.sleep(s, 0);
					} catch (InterruptedException e) {
					}
				}
				setProgress(idx+1, count);
			}
			if(idx == count){
				setMessage(getString(R.string.quad_com_sucess));
			}
			update_value();
			enable(true);
		}
	}
	
	
	private void getVector(){
		mReturn = false;
		MainActivity.getParser().cmd_getVector((byte)mGatherSetting.mSampleType);
	}
	
	float mx,my,mz;
	private void update_value(){
		mx = 0f; my = 0f; mz = 0f;
		int len = 0;
		for(Vector v : result){
			mx += v.x;
			my += v.y;
			mz += v.z;
			len ++;
		}
		if(len>0){
			mx/=(float)len;
			my/=(float)len;
			mz/=(float)len;
			runOnUiThread(new Runnable(){
				public void run() {
					params.get(0).setText(String.format("%f", mx));
					params.get(1).setText(String.format("%f", my));
					params.get(2).setText(String.format("%f", mz));
				}
			});
		}
	}
	
	private String mMessage = null;
	private void setMessage(String msg){
		mMessage = getString(R.string.quad_com_status) + msg;
		runOnUiThread(new Runnable(){
			public void run() {
				info.setText(mMessage);
			}
		});
	}
	
	private boolean mEnable = false;
	private void enable(boolean enable){
		mEnable = enable;
		runOnUiThread(new Runnable(){
			public void run() {
				mSampleOnce.setEnabled(mEnable);
				mSampleMore.setEnabled(mEnable);
				mDataType.setEnabled(mEnable);
			}
		});
	}
	
	private int mProgressPos = 0;
	private int mTotal = 0;
	private void setProgress(int pos, int total){
		mProgressPos = pos;
		mTotal = total;
		runOnUiThread(new Runnable(){
			public void run() {
				float prog = 1.0f;
				if(mProgressPos < mTotal){
					prog = (float)mProgressPos/(float)mTotal;
				}
				QuadParamSettingActivity.setTitleProgress(prog);
			}
		});
	}
	
	void install_listener(){
		MainActivity.setOnReturnVectorListener(onReturnVector);
		MainActivity.setOnReturnMessageListener(onReturnMessage);
	}
	
	void remove_listener(){
		MainActivity.setOnReturnVectorListener(null);
		MainActivity.setOnReturnMessageListener(null);
	}
	
	protected void onResume(){
		install_listener();
		super.onResume();
	}
	
	protected void onDestroy(){
		remove_listener();
		get_setting();
		mGatherSetting.save(this, MainActivity.PRE_NAME);
		super.onDestroy();
	}
	
	
	public static class GatherSetting{
		private static final int DEF_COUNT = 20;
		private static final int DEF_INTERVAL = 50;
		private static final int DEF_TYPE = Protocol.VectorType.RAW_ACC;
		public int mSampleCount = DEF_COUNT;
		public int mSampleInterval = DEF_INTERVAL;
		public int mSampleType = DEF_TYPE;
		GatherSetting(){}

		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putInt("quad_sample_count", mSampleCount);
			editor.putInt("quad_sample_interval", mSampleInterval);
			editor.putInt("quad_sample_type", mSampleType);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mSampleCount = cfg.getInt("quad_sample_count", DEF_COUNT);
			mSampleInterval = cfg.getInt("quad_sample_interval", DEF_INTERVAL);
			mSampleType = cfg.getInt("quad_sample_type", DEF_TYPE);
		}
	}
}
