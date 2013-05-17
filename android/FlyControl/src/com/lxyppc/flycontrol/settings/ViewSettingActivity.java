package com.lxyppc.flycontrol.settings;

import com.lxyppc.flycontrol.MainActivity;
import com.lxyppc.flycontrol.R;
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
	private TextView mTextMargin;
	private SeekBar mSeekBarMargin;
	
	private TextView mTextFreq;
	private SeekBar mSeekBarFreq;
	private TextView mTextStatusDuration;
	private SeekBar mSeekBarStatusDuration;
	private TextView mTextSensitivity;
	private SeekBar mSeekBarSensitivity;
	private EditText mEditWidth;
	private EditText mEditHeight;
	private CheckBox mCheckTextInfo;
	private CheckBox mCheckLockAttitude;
	private CheckBox mCheckLeftThrottle;
	private Button mBtnOK;
	private Button mBtnCancel;
	private ViewSetting oldSetting;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_view_setting);
		//addPreferencesFromResource(R.xml.view_setting);

		mTextStatusDuration = (TextView)this.findViewById(R.id.textStatusDuration);
		mSeekBarStatusDuration = (SeekBar)this.findViewById(R.id.seekStatusDuration);
		mTextVPos = (TextView)this.findViewById(R.id.textVPos);
		mSeekBarVPos = (SeekBar)this.findViewById(R.id.seekVPos);
		mTextMargin = (TextView)this.findViewById(R.id.textMargin);
		mSeekBarMargin = (SeekBar)this.findViewById(R.id.seekMargin);
		
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
		
		oldSetting = new ViewSetting();
		oldSetting.fromIntent(this.getIntent());
		mEditWidth.setText("" + oldSetting.mWidth);
		mEditHeight.setText("" + oldSetting.mHeight);
		mSeekBarVPos.setMax(oldSetting.mHeight-oldSetting.mCtrlHeight);
		mSeekBarVPos.setProgress(oldSetting.mPosition);
		
		mSeekBarMargin.setMax(oldSetting.mWidth/2 -oldSetting.mCtrlWidth);
		mSeekBarMargin.setProgress(oldSetting.mCtrlMargin);
		
		mCheckTextInfo.setChecked(oldSetting.mTextInfo);
		mCheckLockAttitude.setChecked(oldSetting.mLockAttitude);
		mCheckLeftThrottle.setChecked(oldSetting.mLeftThrottle);
		
		mSeekBarStatusDuration.setMax(ViewSetting.DURATION_MAX - ViewSetting.DURATION_OFFSET);
		mSeekBarStatusDuration.setProgress(oldSetting.mStatusDuration);
		
		mSeekBarFreq.setMax(100);
		mSeekBarFreq.setProgress(oldSetting.mFreq);
		
		mSeekBarSensitivity.setMax(100);
		mSeekBarSensitivity.setProgress(oldSetting.mSensitivity);
		
		mTextStatusDuration.setText(getString(R.string.quad_status_duration) + oldSetting.getDuration());
		mTextVPos.setText(getString(R.string.ctrl_v_pos) + oldSetting.mPosition);
		mTextMargin.setText(getString(R.string.ctrl_margin) + oldSetting.mCtrlMargin);
		mTextFreq.setText(getString(R.string.update_freq) + oldSetting.mFreq);
		mTextSensitivity.setText(getString(R.string.attitude_sensitivity) + String.format("%.4f", oldSetting.sensitivity()));
		
		mSeekBarStatusDuration.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onStartTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
				oldSetting.mStatusDuration = arg1;
				mTextStatusDuration.setText(
						ViewSettingActivity.this.getString(R.string.quad_status_duration) + oldSetting.getDuration());
			}
		});
		
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
		
		mSeekBarMargin.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onStartTrackingTouch(SeekBar arg0) {
			}
			@Override
			public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
				mTextMargin.setText(
						ViewSettingActivity.this.getString(R.string.ctrl_margin) + arg1);
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
				s.mCtrlMargin = mSeekBarMargin.getProgress();
				s.mCtrlHeight = oldSetting.mCtrlHeight;
				s.mCtrlWidth = oldSetting.mCtrlWidth;
				s.mFreq = mSeekBarFreq.getProgress();
				s.mStatusDuration = mSeekBarStatusDuration.getProgress();
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
		public static final int DEF_MARGIN = 30;
		public static final int DEF_POSITION = 30;
		public static final int DEF_FREQ = 30;
		public static final int DEF_SENSITIVITY = 50;
		public static final int DURATION_MAX = 2000;
		public static final int DURATION_OFFSET = 50;
		public static final int DEF_DURATION = 1000 - DURATION_OFFSET;
		public int mWidth;
		public int mHeight;
		public int mPosition;
		public int mFreq;
		public boolean mTextInfo;
		public boolean mLockAttitude;
		public boolean mLeftThrottle;
		public int mCtrlHeight;
		public int mCtrlWidth;
		public int mCtrlMargin;
		public int mSensitivity;
		public int mStatusDuration;
		public ViewSetting(){}

		public void fromIntent(Intent i){
			Bundle b = i.getExtras();
			mWidth = b.getInt("width", MainActivity.SCREEN_WIDTH);
			mHeight = b.getInt("height", MainActivity.SCREEN_HEIGHT);
			mCtrlHeight = b.getInt("ctrl_height", 10);
			mCtrlWidth = b.getInt("ctrl_width", 10);
			mTextInfo = b.getBoolean("textinfo", false);
			mPosition = b.getInt("ctrl_postition", DEF_POSITION);
			mCtrlMargin = b.getInt("ctrl_margin", DEF_MARGIN);
			mFreq = b.getInt("update_freq", DEF_FREQ);
			mStatusDuration = b.getInt("status_duration", DEF_DURATION);
			mSensitivity = b.getInt("attitude_sensitivity", DEF_SENSITIVITY);
			mLockAttitude = b.getBoolean("lockattitude", false);
			mLeftThrottle = b.getBoolean("leftthrottle", false);
		}
		
		public void toIntent(Intent i){
			i.putExtra("width", mWidth);
			i.putExtra("height", mHeight);
			i.putExtra("ctrl_height", mCtrlHeight);
			i.putExtra("ctrl_width", mCtrlWidth);
			i.putExtra("textinfo", mTextInfo);
			i.putExtra("ctrl_postition", mPosition);
			i.putExtra("ctrl_margin", mCtrlMargin);
			i.putExtra("update_freq", mFreq);
			i.putExtra("lockattitude", mLockAttitude);
			i.putExtra("leftthrottle", mLeftThrottle);
			i.putExtra("attitude_sensitivity", mSensitivity);
			i.putExtra("status_duration", mStatusDuration);
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putInt("CamereWidth", mWidth);
			editor.putInt("CamereHeight", mHeight);
			editor.putInt("TextInfo", mTextInfo ? 1 : 0);
			editor.putInt("ctrl_postition", mPosition);
			editor.putInt("ctrl_marign", mCtrlMargin);
			editor.putInt("update_freq", mFreq);
			editor.putInt("lockattitude", mLockAttitude ? 1 : 0);
			editor.putInt("leftthrottle", mLeftThrottle ? 1 : 0);
			editor.putInt("attitude_sensitivity", mSensitivity);
			editor.putInt("status_duration", mStatusDuration);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mWidth = cfg.getInt("CamereWidth", MainActivity.SCREEN_WIDTH);
			mHeight = cfg.getInt("CamereHeight", MainActivity.SCREEN_HEIGHT);
			mTextInfo = cfg.getInt("TextInfo", 0) != 0;
			mPosition = cfg.getInt("ctrl_postition", DEF_POSITION);
			mCtrlMargin = cfg.getInt("ctrl_marign", DEF_MARGIN);
			mFreq = cfg.getInt("update_freq", DEF_FREQ);
			mLockAttitude = cfg.getInt("lockattitude", 0) != 0;
			mLeftThrottle = cfg.getInt("leftthrottle", 0) != 0;
			mSensitivity = cfg.getInt("attitude_sensitivity", DEF_SENSITIVITY);
			mStatusDuration = cfg.getInt("status_duration", DEF_DURATION);
		}
		
		public long getDuration(){
			return mStatusDuration + DURATION_OFFSET;
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
