package com.lxyppc.flycontrol.settings;

//import java.lang.reflect.InvocationTargetException;
//import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import com.lxyppc.flycontrol.R;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class BlueSettingActivity extends Activity {
	private EditText mEditMac;
	private CheckBox mCheckConnect;
	private Button mBtnSearch;
	private Button mBtnPair;
	private Button mBtnOK;
	private Button mBtnCancel;
	private LinearLayout mPairedDevices;
	private LinearLayout mNewDevices;
	private static List<String> mNew = new ArrayList<String>();
	private static long lastUpdateTime = 0;
	public static final int REQUESTCODE_BLUETOOTH_ENABLE = 3324;
	
	private BluetoothAdapter mBluetoothAdapter;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS); 
		setContentView(R.layout.activity_blue_setting);
		//addPreferencesFromResource(R.xml.view_setting);
		mEditMac = (EditText)this.findViewById(R.id.editBlueMac);
		mCheckConnect = (CheckBox)this.findViewById(R.id.checkConnectBlue);
		mBtnSearch = (Button)this.findViewById(R.id.btnBlueSearch);
		mBtnOK = (Button)this.findViewById(R.id.blue_setting_ok);
		mBtnCancel = (Button)this.findViewById(R.id.blue_setting_cancel);
		mBtnPair  = (Button)this.findViewById(R.id.blue_setting_pair);
		mPairedDevices = (LinearLayout)this.findViewById(R.id.layoutPairedBlueDevices);
		mNewDevices = (LinearLayout)this.findViewById(R.id.layoutNewBlueDevices);
		mBtnPair.setVisibility(View.INVISIBLE);
		this.setResult(0);
		findViewById(R.id.textNewBlueDevices).setVisibility(View.INVISIBLE);
		findViewById(R.id.textPairedBlueDevices).setVisibility(View.INVISIBLE);
		
		BlueSetting s = new BlueSetting();
		s.fromIntent(this.getIntent());
		mEditMac.setText(s.mMacAddress);
		
		mEditMac.setEnabled(false);
		mCheckConnect.setChecked(s.mConnect);
		
		mBtnOK.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Intent i = new Intent();
				BlueSetting s = new BlueSetting();
				s.mMacAddress = mEditMac.getText().toString();
				s.mConnect = mCheckConnect.isChecked();
				s.toIntent(i);
				
				//Editor ed = getSharedPreferences("bluetooth_settings",Context.MODE_PRIVATE).edit();
				//ed.putString("last_selected_device", s.mMacAddress);
				//ed.putLong("last_selected_device_time", System.currentTimeMillis());
				//ed.putLong("discoverable_end_timestamp", System.currentTimeMillis());
				//ed.commit();
				//mBluetoothAdapter.startDiscovery();
				//mBluetoothAdapter.cancelDiscovery();
				
				BlueSettingActivity.this.setResult(R.id.blue_setting_ok, i);
				BlueSettingActivity.this.finish();
			}
		});
		
		mBtnCancel.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				BlueSettingActivity.this.setResult(R.id.blue_setting_cancel);
				BlueSettingActivity.this.finish();
			}
		});
		
		mBtnSearch.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				BlueSettingActivity.this.setProgressBarIndeterminateVisibility(true);
				
				BlueSettingActivity.this.findViewById(R.id.textNewBlueDevices).setVisibility(View.VISIBLE);
				
				if (mBluetoothAdapter.isDiscovering()) {
					mBluetoothAdapter.cancelDiscovery();
				}
				
				mBtnSearch.setEnabled(false);
				mNewDevices.removeAllViews();
				mNew.clear();
				mBluetoothAdapter.startDiscovery();
			}
		});
		
		mBtnPair.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				//BlueSetting s = new BlueSetting();
				//s.mMacAddress = mEditMac.getText().toString();
				//s.mConnect = true;
				//check_bond(s);
			}
		});
		
		// Register for broadcasts when a device is discovered
		IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		this.registerReceiver(this.mReceiver, filter);

		// Register for broadcasts when discovery has finished
		filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		this.registerReceiver(this.mReceiver, filter);
				
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		
		if(mBluetoothAdapter == null){
			Toast.makeText(this, getString(R.string.blue_not_valid), Toast.LENGTH_LONG).show();
			this.finish();
		}
		
		if(!mBluetoothAdapter.isEnabled()){
			Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			BlueSettingActivity.this.startActivityForResult(enableIntent, REQUESTCODE_BLUETOOTH_ENABLE);
		}
		
		Set<BluetoothDevice> pairedDevices = this.mBluetoothAdapter.getBondedDevices();
		if (pairedDevices.size() > 0) {
			findViewById(R.id.textPairedBlueDevices).setVisibility(View.VISIBLE);
			for (final BluetoothDevice device : pairedDevices) {
				add_item(this.mPairedDevices,blueString(device));
			}
		} else {
			add_item(this.mPairedDevices,getString(R.string.blue_no_device_paired));
			//mPairedDevicesArrayAdapter.add("No devices have been paired!");
		}
		
		
		if(lastUpdateTime + 30*1000 < System.currentTimeMillis()){
			// time out, need re-scan devices
			mNew.clear();
		}
		update_new_item();
	}
	
	protected void onActivityResult(final int pRequestCode, final int pResultCode, final Intent pData) {
		switch(pRequestCode) {
			case REQUESTCODE_BLUETOOTH_ENABLE:
				mPairedDevices.removeAllViewsInLayout();
				Set<BluetoothDevice> pairedDevices = this.mBluetoothAdapter.getBondedDevices();
				if (pairedDevices.size() > 0) {
					findViewById(R.id.textPairedBlueDevices).setVisibility(View.VISIBLE);
					for (final BluetoothDevice device : pairedDevices) {
						add_item(this.mPairedDevices,blueString(device));
					}
				} else {
					add_item(this.mPairedDevices,getString(R.string.blue_no_device_paired));
					//mPairedDevicesArrayAdapter.add("No devices have been paired!");
				}
				break;
		}
	}
	
	String blueString(BluetoothDevice device){
		return device.getName() + 
				"(class:"+device.getBluetoothClass().toString()+")\n" + 
				device.getAddress();
	}
	
	private boolean mColor = false;
	protected void add_item(LinearLayout layout, String content){
		TextView view = new TextView(this);
		view.setText(content);
		view.setBackgroundResource(mColor?R.color.text_bg_1:R.color.text_bg_2);
		mColor = !mColor;
		view.setOnClickListener(mDeviceClickListener);
		//LinearLayout ll = new LinearLayout(this);
		//LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
		//lp.weight = 1;
		//ll.addView(view);
		layout.addView(view);
	}
	
	protected void update_new_item(){
		mNewDevices.removeAllViews();
		if(mNew.size() > 0){
			findViewById(R.id.textNewBlueDevices).setVisibility(View.VISIBLE);
		}
		for(String s:mNew){
			add_item(mNewDevices,s);
		}
	}
	
	
	private View.OnClickListener mDeviceClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View pView) {
			BlueSettingActivity.this.mBluetoothAdapter.cancelDiscovery();
			final String info = ((TextView) pView).getText().toString();
			final String address = info.substring(info.length() - 17);
			mEditMac.setText(address);
		}
	};
	
	private boolean mDeviceFind = false;
	private BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(final Context pContext, final Intent pIntent) {
			final String action = pIntent.getAction();

			if (action.equals(BluetoothDevice.ACTION_FOUND)) {
				final BluetoothDevice device = pIntent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
				if (device.getBondState() != BluetoothDevice.BOND_BONDED) {
					mNew.add(blueString(device));
					add_item(mNewDevices, blueString(device));
					mDeviceFind = true;
				}
			} else if (action.equals(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)) {
				BlueSettingActivity.this.setProgressBarIndeterminateVisibility(false);
				mBtnSearch.setEnabled(true);
				lastUpdateTime = System.currentTimeMillis();
				if (!mDeviceFind) {
					add_item(mNewDevices, getString(R.string.blue_no_device_found));
				}
			}
		}
	};
	/*
	protected void check_bond(BlueSetting mBlueSetting){
		if(!mBlueSetting.mConnect) return;
		BluetoothDevice blueDev = mBluetoothAdapter.getRemoteDevice(mBlueSetting.mMacAddress);
		if(blueDev.getBondState() == BluetoothDevice.BOND_NONE){
			Class class1;
			try {
				class1 = Class.forName("android.bluetooth.BluetoothDevice");
				Method createBondMethod = class1.getMethod("createBond");
				Boolean returnValue = (Boolean) createBondMethod.invoke(blueDev);
				returnValue.booleanValue();
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (SecurityException e) {
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			}
			
		}
	}
	*/
	protected void onDestroy() {
		super.onDestroy();

		// Make sure we're not doing discovery anymore
		if (this.mBluetoothAdapter != null) {
			this.mBluetoothAdapter.cancelDiscovery();
		}

		// Unregister broadcast listeners
		this.unregisterReceiver(this.mReceiver);
	}
	
	public static class BlueSetting{
		//public static final String UUID = "49317f50-a702-11e2-9e96-0800200c9a66";
		public static final String UUID = "00001101-0000-1000-8000-00805F9B34FB";
		public static final String DEF_MAC = "00-00-00-00-00-00";
		public String mMacAddress;
		public boolean mConnect;
		public BlueSetting(){}
		BlueSetting(BlueSetting s ){
			mMacAddress = s.mMacAddress;
			mConnect = s.mConnect;
		}
		public void fromIntent(Intent i){
			Bundle b = i.getExtras();
			mMacAddress = b.getString("bluemacaddress");
			mConnect = b.getBoolean("blueconnect", false);
		}
		
		public void toIntent(Intent i){
			i.putExtra("bluemacaddress", mMacAddress);
			i.putExtra("blueconnect", mConnect);
		}
		
		public void save(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			Editor editor = cfg.edit();
			editor.putString("bluemacaddress", mMacAddress);
			editor.putInt("blueconnect", mConnect ? 1 : 0);
			editor.commit();
		}
		
		public void restore(Context context, String preferencesName){
			SharedPreferences cfg = context.getSharedPreferences(preferencesName, MODE_PRIVATE);
			mMacAddress = cfg.getString("bluemacaddress", DEF_MAC);
			mConnect = cfg.getInt("blueconnect", 1) != 0;
		}
	}
}
