package com.lxyppc.flycontrol;

import android.app.Activity;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.LinearLayout;
public class PIDSettingActivity extends Activity {

	
	private QuadParamSettingActivity.ParamEdit params;
	private QuadParamSettingActivity.ParamButton buttons;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_pid_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.pid_param_layout);
		
		params = new QuadParamSettingActivity.ParamEdit(this, 3, 3);
		params.append2layout(tl, R.array.quad_pid_param_head, R.array.quad_pid_param_names);
		buttons = new QuadParamSettingActivity.ParamButton(this, 23, 9, params);
		buttons.append2layout(tl);
	}
	
	public boolean onContextItemSelected(MenuItem item) {
		params.onContextItemSelected(item);
		return super.onContextItemSelected(item);
	}
	
	protected void onResume(){
		buttons.install_listener();
		super.onResume();
	}
	
	protected void onDestroy(){
		buttons.remove_listener();
		super.onDestroy();
	}
}
