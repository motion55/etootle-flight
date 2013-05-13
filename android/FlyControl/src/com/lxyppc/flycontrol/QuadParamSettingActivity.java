package com.lxyppc.flycontrol;

import android.app.TabActivity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TabHost;
import android.widget.TabHost.TabSpec;

public class QuadParamSettingActivity extends TabActivity {
	private static final String SENSOR_TAB = "sensor_tab";
	private static final String PID_TAB = "pid_tab";
	private static final String HORIZON_TAB = "horizon_tab";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
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
		
	}
}
