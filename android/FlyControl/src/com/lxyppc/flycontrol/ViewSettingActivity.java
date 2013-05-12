package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

public class ViewSettingActivity extends Activity {
	
	private TextView mTextVPos;
	private SeekBar mSeekBarVPos;
	private TextView mTextFreq;
	private SeekBar mSeekBarFreq;
	private TextView mTextSensitivity;
	private SeekBar mSeekBarSensitivity;
	private EditText mEditWidth;
	private EditText mEditHeight;
	private CheckBox mCheckTextInfo;
	private CheckBox mCheckLockAttitude;
	private CheckBox mCheckLeftThrottle;
	private Button mBtnOK;
	private Button mBtnCancel;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_view_setting);
		//addPreferencesFromResource(R.xml.view_setting);
		mTextVPos = (TextView)this.findViewById(R.id.textVPos);
		mSeekBarVPos = (SeekBar)this.findViewById(R.id.seekVPos);
		mTextFreq = (TextView)this.findViewById(R.id.textFreq);
		mSeekBarFreq = (SeekBar)this.findViewById(R.id.seekFreq);
		mTextSensitivity = (TextView)this.findViewById(R.id.textSensitivity);
		mSeekBarSensitivity = (SeekBar)this.findViewById(R.id.seekSensitivity);
		mEditWidth = (EditText)this.findViewById(R.id.editWidth);
		mEditHeight = (EditText)this.findViewById(R.id.editHeight);
		mCheckTextInfo = (CheckBox)this.findViewById(R.id.checkTextInfo);
		mBtnOK = (Button)this.findViewById(R.id.view_setting_ok);
		mBtnCancel = (Button)this.findViewById(R.id.view_setting_cancel);
		mCheckLockAttitude = (CheckBox)this.findViewById(R.id.checkLockAttitude);
		mCheckLeftThrottle = (CheckBox)this.findViewById(R.id.checkLeftThrottle);
		
		ViewSetting s = new ViewSetting();
		s.fromIntent(this.getIntent());
		mEditWidth.setText("" + s.mWidth);
		mEditHeight.setText("" + s.mHeight);
		mSeekBarVPos.setMax(s.mHeight-s.mCtrlHeight);
		mCheckTextInfo.setChecked(s.mTextInfo);
		mCheckLockAttitude.setChecked(s.mLockAttitude);
		mCheckLeftThrottle.setChecked(s.mLeftThrottle);
		mSeekBarVPos.setProgress(s.mPosition);
		
		mSeekBarFreq.setMax(100);
		mSeekBarFreq.setProgress(s.mFreq);
		
		mSeekBarSensitivity.setMax(100);
		mSeekBarSensitivity.setProgress(s.mSensitivity);
		
		mTextVPos.setText(getString(R.string.ctrl_v_pos) + s.mPosition);
		mTextFreq.setText(getString(R.string.update_freq) + s.mFreq);
		mTextSensitivity.setText(getString(R.string.attitude_sensitivity) + String.format("%.4f", s.sensitivity()));
		
		mSeekBarVPos.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onStartTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
				mTextVPos.setText(
						ViewSettingActivity.this.getString(R.string.ctrl_v_pos) + arg1);
			}
		});
		
		mSeekBarFreq.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onStartTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
				mTextFreq.setText(
						ViewSettingActivity.this.getString(R.string.update_freq) + arg1);
			}
		});
		
		mSeekBarSensitivity.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onStartTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
				mTextSensitivity.setText(
						ViewSettingActivity.this.getString(R.string.attitude_sensitivity) + 
						String.format("%.4f", ViewSetting.sensitivity(arg1)));
			}
		});
		
		mBtnOK.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Intent i = new Intent();
				ViewSetting s = new ViewSetting();
				s.mWidth = Integer.parseInt(mEditWidth.getText().toString());
				s.mHeight = Integer.parseInt(mEditHeight.getText().toString());
				s.mTextInfo = mCheckTextInfo.isChecked();
				s.mLockAttitude = mCheckLockAttitude.isChecked();
				s.mLeftThrottle = mCheckLeftThrottle.isChecked();
				s.mPosition = mSeekBarVPos.getProgress();
				s.mCtrlHeight = 10;
				s.mFreq = mSeekBarFreq.getProgress();
				if(s.mFreq <= 0) s.mFreq = 1;
				s.mSensitivity = mSeekBarSensitivity.getProgress();
				s.toIntent(i);
				ViewSettingActivity.this.setResult(R.id.view_setting_ok, i);
				ViewSettingActivity.this.finish();
			}
		});
		
		mBtnCancel.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				ViewSettingActivity.this.setResult(R.id.view_setting_cancel);
				ViewSettingActivity.this.finish();
			}
		});
	}
	
	public static class ViewSetting{
		private static final int CAMERA_WIDTH = 320;
		private static final int CAMERA_HEIGHT = 240;
		public int mWidth;
		public int mHeight;
		public int mPosition;
		public int mFreq;
		public boolean mTextInfo;
		public boolean mLockAttitude;
		public boolean mLeftThrottle;
		public int mCtrlHeight;
		public int mSensitivity;
		ViewSetting(){}
		ViewSetting(ViewSetting s ){
			mWidth = s.mWidth;
			mHeight = s.mHeight;
			mPosition = s.mPosition;
			mTextInfo = s.mTextInfo;
			mCtrlHeight = s.mCtrlHeight;
			mFreq = s.mFreq;
			mLockAttitude = s.mLockAttitude;
			mLeftThrottle = s.mLeftThrottle;
			mSensitivity = s.mSensitivity;
		}
		ViewSetting(int width, int height, int position, boolean textinfo){
			mWidth = width;
			mHeight = height;
			mPosition = position;
			mTextInfo = textinfo;
			mCtrlHeight = 10;
			mFreq = 20;
			mSensitivity = 50;
		}
		ViewSetting(int width, int height, int position, boolean textinfo, int ctrlHeight){
			mWidth = width;
			mHeight = height;
			mPosition = position;
			mTextInfo = textinfo;
			mCtrlHeight = ctrlHeight;
		}
		public void fromIntent(Intent i){
			Bundle b = i.getExtras();
			mWidth = b.getInt("width", 320);
			mHeight = b.getInt("height", 240);
			mCtrlHeight = b.getInt("ctrl_height", 10);
			mTextInfo = b.getBoolean("textinfo", false);
			mPosition = b.getInt("postition", 10);
			mFreq = b.getInt("update_freq", 20);
			mSensitivity = b.getInt("attitude_sensitivity", 50);
			mLockAttitude = b.getBoolean("lockattitude", false);
			mLeftThrottle = b.getBoolean("leftthrottle", false);
		}
		
		public void toIntent(Intent i){
			i.putExtra("width", mWidth);
			i.putExtra("height", mHeight);
			i.putExtra("ctrl_height", mCtrlHeight);
			i.putExtra("textinfo", mTextInfo);
			i.putExtra("postition", mPosition);
			i.putExtra("update_freq", mFreq);
			i.putExtra("lockattitude", mLockAttitude);
			i.putExtra("leftthrottle", mLeftThrottle);
			i.putExtra("attitude_sensitivity", mSensitivity);
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putInt("CamereWidth", mWidth);
			editor.putInt("CamereHeight", mHeight);
			editor.putInt("TextInfo", mTextInfo ? 1 : 0);
			editor.putInt("VPosition", mPosition);
			editor.putInt("update_freq", mFreq);
			editor.putInt("lockattitude", mLockAttitude ? 1 : 0);
			editor.putInt("leftthrottle", mLeftThrottle ? 1 : 0);
			editor.putInt("attitude_sensitivity", mSensitivity);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mWidth = cfg.getInt("CamereWidth", CAMERA_WIDTH);
			mHeight = cfg.getInt("CamereHeight", CAMERA_HEIGHT);
			mTextInfo = cfg.getInt("TextInfo", 0) != 0;
			mPosition = cfg.getInt("VPosition", 0);
			mFreq = cfg.getInt("update_freq", 20);
			mLockAttitude = cfg.getInt("lockattitude", 0) != 0;
			mLeftThrottle = cfg.getInt("leftthrottle", 0) != 0;
			mSensitivity = cfg.getInt("attitude_sensitivity", 50);
		}
		
		public static float sensitivity(int val){
			if(val == 0) return 0.0001f;
			return (float)val * 0.0001f;
		}
		
		public float sensitivity(){
			return sensitivity(mSensitivity);
		}
	}
}
