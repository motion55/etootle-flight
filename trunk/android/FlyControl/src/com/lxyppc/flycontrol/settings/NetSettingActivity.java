package com.lxyppc.flycontrol.settings;

import com.lxyppc.flycontrol.MyMath;
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

public class NetSettingActivity extends Activity {
	private EditText mEditIP;
	private EditText mEditPort;
	private CheckBox mCheckConnectServer;
	private Button mBtnOK;
	private Button mBtnCancel;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_net_setting);
		//addPreferencesFromResource(R.xml.view_setting);
		mEditIP = (EditText)this.findViewById(R.id.editIPAddress);
		mEditPort = (EditText)this.findViewById(R.id.editIPPort);
		mCheckConnectServer = (CheckBox)this.findViewById(R.id.checkConnectServer);
		mBtnOK = (Button)this.findViewById(R.id.net_setting_ok);
		mBtnCancel = (Button)this.findViewById(R.id.net_setting_cancel);
		
		NetSetting s = new NetSetting();
		s.fromIntent(this.getIntent());
		mEditIP.setText(s.mIP);
		mEditPort.setText("" + s.mPort);
		mCheckConnectServer.setChecked(s.mConnectServer);
		
		mBtnOK.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Intent i = new Intent();
				NetSetting s = new NetSetting();
				s.mIP = mEditIP.getText().toString();
				s.mPort = MyMath.toInt(mEditPort.getText().toString());
				s.mConnectServer = mCheckConnectServer.isChecked();
				s.toIntent(i);
				NetSettingActivity.this.setResult(R.id.net_setting_ok, i);
				NetSettingActivity.this.finish();
			}
		});
		
		mBtnCancel.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				NetSettingActivity.this.setResult(R.id.net_setting_cancel);
				NetSettingActivity.this.finish();
			}
		});
	}
	
	public static class NetSetting{
		private static final int DEF_PORT = 31217;
		private static final String DEF_IP = "127.0.0.1";
		public String mIP;
		public int mPort;
		public boolean mConnectServer;
		public NetSetting(){}
		NetSetting(NetSetting s ){
			mIP = s.mIP;
			mPort = s.mPort;
			mConnectServer = s.mConnectServer;
		}
		NetSetting(String IP, int port, boolean connectServer){
			mIP = IP;
			mPort = port;
			mConnectServer = connectServer;
		}
		public void fromIntent(Intent i){
			Bundle b = i.getExtras();
			mIP = b.getString("ipaddress");
			mPort = b.getInt("port", DEF_PORT);
			mConnectServer = b.getBoolean("connectserver", false);
		}
		
		public void toIntent(Intent i){
			i.putExtra("ipaddress", mIP);
			i.putExtra("port", mPort);
			i.putExtra("connectserver", mConnectServer);
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putString("ipaddres", mIP);
			editor.putInt("ipport", mPort);
			editor.putInt("connectserver", mConnectServer ? 1 : 0);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mIP = cfg.getString("ipaddres", DEF_IP);
			mPort = cfg.getInt("ipport", DEF_PORT);
			mConnectServer = cfg.getInt("connectserver", 1) != 0;
		}
	}
}
