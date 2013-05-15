package com.lxyppc.flycontrol;

import android.app.Activity;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.LinearLayout;
public class HorizonSettingActivity extends Activity {
	
	private QuadParamSettingActivity.ParamEdit params;
	private QuadParamSettingActivity.ParamButton buttons;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_horizon_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.horizon_param_layout);

		params = new QuadParamSettingActivity.ParamEdit(this, 1, 4);
		params.append2layout(tl, R.array.quad_horizon_param_head, R.array.quad_horizon_param_names);
		buttons = new QuadParamSettingActivity.ParamButton(this, 19, 4, params);
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
