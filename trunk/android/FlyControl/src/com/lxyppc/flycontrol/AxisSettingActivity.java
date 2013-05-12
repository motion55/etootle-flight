package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class AxisSettingActivity extends Activity {
	
	
	private EditText mX1S;
	private EditText mX1O;
	private EditText mY1S;
	private EditText mY1O;
	private EditText mX2S;
	private EditText mX2O;
	private EditText mY2S;
	private EditText mY2O;
	private Button mBtnOK;
	private Button mBtnCancel;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_axis_setting);
		//addPreferencesFromResource(R.xml.view_setting);
		mX1S = (EditText)this.findViewById(R.id.editLeftXScale);
		mX1O = (EditText)this.findViewById(R.id.editLeftXOffset);
		mY1S = (EditText)this.findViewById(R.id.editLeftYScale);
		mY1O = (EditText)this.findViewById(R.id.editLeftYOffset);
		mX2S = (EditText)this.findViewById(R.id.editRightXScale);
		mX2O = (EditText)this.findViewById(R.id.editRightXOffset);
		mY2S = (EditText)this.findViewById(R.id.editRightYScale);
		mY2O = (EditText)this.findViewById(R.id.editRightYOffset);
		
		mBtnOK = (Button)this.findViewById(R.id.axis_setting_ok);
		mBtnCancel = (Button)this.findViewById(R.id.axis_setting_cancel);
		
		AxisSetting s = new AxisSetting();
		s.fromIntent(this.getIntent());
		mX1S.setText(""+s.mLeftXScale);
		mX1O.setText(""+s.mLeftXOffset);
		mY1S.setText(""+s.mLeftYScale);
		mY1O.setText(""+s.mLeftYOffset);
		mX2S.setText(""+s.mRightXScale);
		mX2O.setText(""+s.mRightXOffset);
		mY2S.setText(""+s.mRightYScale);
		mY2O.setText(""+s.mRightYOffset);
		
		mBtnOK.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Intent i = new Intent();
				AxisSetting s = new AxisSetting();
				s.mLeftXScale = Integer.parseInt(mX1S.getText().toString());
				s.mLeftXOffset = Integer.parseInt(mX1O.getText().toString());
				s.mLeftYScale = Integer.parseInt(mY1S.getText().toString());
				s.mLeftYOffset = Integer.parseInt(mY1O.getText().toString());
				s.mRightXScale = Integer.parseInt(mX2S.getText().toString());
				s.mRightXOffset = Integer.parseInt(mX2O.getText().toString());
				s.mRightYScale = Integer.parseInt(mY2S.getText().toString());
				s.mRightYOffset = Integer.parseInt(mY2O.getText().toString());
				s.toIntent(i);
				AxisSettingActivity.this.setResult(R.id.axis_setting_ok, i);
				AxisSettingActivity.this.finish();
			}
		});
		
		mBtnCancel.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				AxisSettingActivity.this.setResult(R.id.axis_setting_cancel);
				AxisSettingActivity.this.finish();
			}
		});
	}
	
	public static class AxisSetting{
		private static final int OFFSET = 0;
		private static final int SCALE = 2000;
		public int mLeftXScale;
		public int mLeftXOffset;
		public int mLeftYScale;
		public int mLeftYOffset;
		public int mRightXScale;
		public int mRightXOffset;
		public int mRightYScale;
		public int mRightYOffset;
		AxisSetting(){}
		AxisSetting(AxisSetting s ){
			mLeftXScale = s.mLeftXScale;
			mLeftXOffset = s.mLeftXOffset;
			mLeftYScale = s.mLeftYScale;
			mLeftYOffset = s.mLeftYOffset;
			mRightXScale = s.mRightXScale;
			mRightXOffset = s.mRightXOffset;
			mRightYScale = s.mRightYScale;
			mRightYOffset = s.mRightYOffset;
		}

		public void fromIntent(Intent i){
			Bundle b = i.getExtras();
			mLeftXScale = b.getInt("leftxscale", SCALE);
			mLeftXOffset = b.getInt("leftxoffet", OFFSET);
			mLeftYScale = b.getInt("leftyscale", SCALE);
			mLeftYOffset = b.getInt("leftyoffset", OFFSET);
			mRightXScale = b.getInt("rightxscale", SCALE);
			mRightXOffset = b.getInt("rightxoffset", OFFSET);
			mRightYScale = b.getInt("rightyscale", SCALE);
			mRightYOffset = b.getInt("rightyoffset", OFFSET);
		}
		
		public void toIntent(Intent i){
			i.putExtra("leftxscale", mLeftXScale);
			i.putExtra("leftxoffet", mLeftXOffset);
			i.putExtra("leftyscale", mLeftYScale);
			i.putExtra("leftyoffset", mLeftYOffset);
			i.putExtra("rightxscale", mRightXScale);
			i.putExtra("rightxoffset", mRightXOffset);
			i.putExtra("rightyscale", mRightYScale);
			i.putExtra("rightyoffset", mRightYOffset);
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putInt("leftxscale", mLeftXScale);
			editor.putInt("leftxoffet", mLeftXOffset);
			editor.putInt("leftyscale", mLeftYScale);
			editor.putInt("leftyoffset", mLeftYOffset);
			editor.putInt("rightxscale", mRightXScale);
			editor.putInt("rightxoffset", mRightXOffset);
			editor.putInt("rightyscale", mRightYScale);
			editor.putInt("rightyoffset", mRightYOffset);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mLeftXScale = cfg.getInt("leftxscale", SCALE);
			mLeftXOffset = cfg.getInt("leftxoffet", OFFSET);
			mLeftYScale = cfg.getInt("leftyscale", SCALE);
			mLeftYOffset = cfg.getInt("leftyoffset", OFFSET);
			mRightXScale = cfg.getInt("rightxscale", SCALE);
			mRightXOffset = cfg.getInt("rightxoffset", OFFSET);
			mRightYScale = cfg.getInt("rightyscale", SCALE);
			mRightYOffset = cfg.getInt("rightyoffset", OFFSET);
		}
	}
}
