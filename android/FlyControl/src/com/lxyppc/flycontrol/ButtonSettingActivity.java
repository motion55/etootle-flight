package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.support.v4.view.ViewPager.LayoutParams;
import android.text.InputType;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ButtonSettingActivity extends Activity {
	
	class Controls{
		Controls(CheckBox c, EditText x, EditText y, EditText v){
			enable = c;
			this.x = x;
			this.y = y;
			this.value = v;
		}
		CheckBox enable;
		EditText x;
		EditText y;
		EditText value;
	}
	Controls[] mControls = new Controls[ButtonSetting.MAX_BUTTON_CNT];
	CheckBox adjMode;
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_button_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.button_setting_layout);
		tl.removeAllViews();
		{
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			TextView v2 = new TextView(this);
			TextView v3 = new TextView(this);
			TextView v4 = new TextView(this);
			v1.setText(R.string.btn_enable);
			v2.setText(R.string.btn_x);
			v3.setText(R.string.btn_y);
			v4.setText(R.string.btn_name);
			ll.addView(v1,lp);
			ll.addView(v2,lp);
			ll.addView(v3,lp);
			ll.addView(v4,lp);
			tl.addView(ll);
		}
		ButtonSetting s = new ButtonSetting();
		s.fromIntent(getIntent());
		for(int i=0;i<ButtonSetting.MAX_BUTTON_CNT;i++){
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 1;
			CheckBox chk = new CheckBox(this);
			chk.setChecked(s.mButtonSettings[i].mEnable);
			chk.setText(getString(R.string.button) + (i + 1));
			ll.addView(chk,lp);
			
			EditText editX = new  EditText(this);
			editX.setText(""+s.mButtonSettings[i].x);
			editX.setInputType(InputType.TYPE_CLASS_NUMBER);
			EditText editY = new  EditText(this);
			editY.setText(""+s.mButtonSettings[i].y);
			editY.setInputType(InputType.TYPE_CLASS_NUMBER);
			EditText editName = new  EditText(this);
			editName.setText(s.mButtonSettings[i].value);
			
			mControls[i] = new Controls(chk,editX,editY,editName);
			ll.addView(editX,lp);
			ll.addView(editY,lp);
			ll.addView(editName,lp);
			//lv.addView(ll);
			tl.addView(ll);
		}
		{
			adjMode = new CheckBox(this);
			adjMode.setChecked(s.mAdjMode);
			adjMode.setText(R.string.btn_adj_pos);
			tl.addView(adjMode);
		}
		LinearLayout ll = new LinearLayout(this);
		Button btnOK = new Button(this);
		btnOK.setId(R.id.button_setting_ok);
		btnOK.setText(android.R.string.ok);
		Button btnCancel = new Button(this);
		btnCancel.setId(R.id.button_setting_cancel);
		btnCancel.setText(android.R.string.cancel);
		LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
		lp.weight = 1;
		ll.addView(btnOK,lp);
		ll.addView(btnCancel,lp);
		tl.addView(ll);
		btnOK.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View arg0) {
				Intent intent = new Intent();
				ButtonSetting s = new ButtonSetting();
				s.mAdjMode = adjMode.isChecked();
				for(int i=0;i<mControls.length;i++){
					s.mButtonSettings[i].mEnable = mControls[i].enable.isChecked();
					s.mButtonSettings[i].x = Integer.parseInt(mControls[i].x.getText().toString());
					s.mButtonSettings[i].y = Integer.parseInt(mControls[i].y.getText().toString());
					s.mButtonSettings[i].value = mControls[i].value.getText().toString();
				}
				s.toIntent(intent);
				ButtonSettingActivity.this.setResult(R.id.button_setting_ok, intent);
				ButtonSettingActivity.this.finish();
			}
			
		});
		
		btnCancel.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				ButtonSettingActivity.this.setResult(R.id.button_setting_cancel);
				ButtonSettingActivity.this.finish();
			}
			
		});
	}
	
	public static class ButtonSetting{
		public static final int MAX_BUTTON_CNT = 10;
		public static class BtnSet{
			public boolean mEnable;
			public int x;
			public int y;
			public String value;
			public int state;
			public boolean is_float(){ return x<0 || y<0;}
			public void toggle(){ state = state == 0 ? 1 : 0; }
		};
		public int mButtonCount;
		public boolean mAdjMode = false;
		public BtnSet[] mButtonSettings = new BtnSet[MAX_BUTTON_CNT];
		ButtonSetting(){
			for(int i=0;i<mButtonSettings.length;i++){
				mButtonSettings[i] = new BtnSet();
			}
		}
		ButtonSetting(ButtonSetting s ){
			for(int i=0;i<mButtonSettings.length;i++){
				mButtonSettings[i] = new BtnSet();
			}
			mButtonCount = s.mButtonCount;
			for(int i=0;i<mButtonSettings.length;i++){
				mButtonSettings[i].mEnable = s.mButtonSettings[i].mEnable;
				mButtonSettings[i].x = s.mButtonSettings[i].x;
				mButtonSettings[i].y = s.mButtonSettings[i].y;
				mButtonSettings[i].value = s.mButtonSettings[i].value;
				mButtonSettings[i].state = s.mButtonSettings[i].state;
			}
		}

		public void fromIntent(Intent intent){
			Bundle b = intent.getExtras();
			mButtonCount = b.getInt("buttoncount", 0);
			mAdjMode = b.getBoolean("adjmode", false);
			for(int i=0;i<mButtonSettings.length;i++){
				mButtonSettings[i].mEnable = b.getBoolean("btnenable" + i, false);
				mButtonSettings[i].x = b.getInt("btnx"+i, -1);
				mButtonSettings[i].y = b.getInt("btny"+i, -1);
				mButtonSettings[i].value = b.getString("btnvalue"+i);
				mButtonSettings[i].state = b.getInt("btnstate"+i, 0);
			}
		}
		
		public void toIntent(Intent intent){
			intent.putExtra("buttoncount", mButtonCount);
			intent.putExtra("adjmode", mAdjMode);
			for(int i=0;i<mButtonSettings.length;i++){
				intent.putExtra("btnenable"+i, mButtonSettings[i].mEnable);
				intent.putExtra("btnx"+i, mButtonSettings[i].x);
				intent.putExtra("btny"+i, mButtonSettings[i].y);
				intent.putExtra("btnvalue"+i, mButtonSettings[i].value);
				intent.putExtra("btnstate"+i, mButtonSettings[i].state);
			}
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putInt("buttoncount", mButtonCount);
			editor.putInt("adjmode", mAdjMode?1:0);
			for(int i=0;i<mButtonSettings.length;i++){
				editor.putInt("btnenable"+i, mButtonSettings[i].mEnable?1:0);
				editor.putInt("btnx"+i, mButtonSettings[i].x);
				editor.putInt("btny"+i, mButtonSettings[i].y);
				editor.putString("btnvalue"+i, mButtonSettings[i].value);
				editor.putInt("btnstate"+i, mButtonSettings[i].state);
			}
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mButtonCount = cfg.getInt("buttoncount", 0);
			mAdjMode = cfg.getInt("adjmode", 0) != 0;
			for(int i=0;i<mButtonSettings.length;i++){
				mButtonSettings[i].mEnable = cfg.getInt("btnenable" + i,0) != 0;
				mButtonSettings[i].x = cfg.getInt("btnx"+i,-1);
				mButtonSettings[i].y = cfg.getInt("btny"+i,-1);
				mButtonSettings[i].value = cfg.getString("btnvalue"+i,"");
				mButtonSettings[i].state = cfg.getInt("btnstate"+i,0);
			}
		}
		
		public int unfixed_count(){
			int r = 0;
			for(BtnSet s:mButtonSettings){
				if(s.mEnable && (s.x < 0 || s.y < 0) ){
					r++;
				}
			}
			return r;
		}
		
		public int count(){
			int r = 0;
			for(BtnSet s:mButtonSettings){
				if(s.mEnable){
					r++;
				}
			}
			return r;
		}
		
		public boolean moveable(){
			return mAdjMode;
		}
		
		public int total_count(){
			return mButtonSettings.length;
		}
	}
}
