package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.ContextMenu;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ContextMenu.ContextMenuInfo;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;
public class SensorSettingActivity extends Activity {
	public class SensorParam{
		public static final int COUNT = 3;
		SensorParam(Context ctx, View.OnCreateContextMenuListener l){
			for(int i=0;i<COUNT;i++){
			edit[i] = new EditText(ctx);
			edit[i].setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			edit[i].setOnCreateContextMenuListener(l);
			}
		}
		EditText[] edit = new EditText[COUNT];
	}

	private static final int IDX_BASE = 10000;
	private View.OnCreateContextMenuListener mCreateContextMenuListener= new View.OnCreateContextMenuListener(){
		@Override
		public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
			int index = -1;
			for(int i=0;i<params.length;i++){
				for(int j=0;j<params[0].edit.length;j++){
					if(v == params[i].edit[j]){
						index = i*3+j;
						break;
					}
				}
				if(index>0) break;
			}
			if(index > 0){
				index += IDX_BASE;
				menu.add(0, index, Menu.NONE, "X Fill");
				menu.add(1, index, Menu.NONE, "Y Fill");
			}
		}
	};
	private SensorParam[] params = new SensorParam[6];
	private EditText[] edits = new EditText[18];
	private QuadParamSettingActivity.ParamButton buttons;
	
	public boolean onContextItemSelected(MenuItem item) {
		int id = item.getItemId();
		if(id>=IDX_BASE && id<IDX_BASE+18){
			id -= IDX_BASE;
			int row = id/SensorParam.COUNT;
			int col = id%SensorParam.COUNT;
			String v = params[row].edit[col].getText().toString();
			if(item.getGroupId() == 0){
				// X fill
				for(int i=0;i<SensorParam.COUNT;i++){
					params[row].edit[i].setText(v);
				}
			}else{
				// Y fill
				for(int i=0;i<params.length;i++){
					params[i].edit[col].setText(v);
				}
			}
		}
		return super.onContextItemSelected(item);
	}

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sensor_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.sensor_param_layout);
		String[] name = new String[]{
			getString(R.string.quad_acc_offset),
			getString(R.string.quad_acc_gain),
			getString(R.string.quad_gyro_offset),
			getString(R.string.quad_gyro_gain),
			getString(R.string.quad_mag_offset),
			getString(R.string.quad_mag_gain),
		};
		{
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			TextView v2 = new TextView(this);
			TextView v3 = new TextView(this);
			TextView v4 = new TextView(this);
			v1.setText(R.string.quad_sensor_cal);
			v2.setText(R.string.quad_sensor_x);
			v3.setText(R.string.quad_sensor_y);
			v4.setText(R.string.quad_sensor_z);
			ll.addView(v1,lp);
			ll.addView(v2,lp);
			ll.addView(v3,lp);
			ll.addView(v4,lp);
			tl.addView(ll);
		}
		for(int i=0;i<name.length;i++){
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			v1.setText(name[i]);
			params[i] = new SensorParam(this, mCreateContextMenuListener);
			ll.addView(v1,lp);
			for(int j=0;j<SensorParam.COUNT;j++){
				ll.addView(params[i].edit[j],lp);
				edits[i*SensorParam.COUNT + j] = params[i].edit[j];
			}
			tl.addView(ll);
		}
		buttons = new QuadParamSettingActivity.ParamButton(this, 1, 18, edits);
		buttons.append2layout(tl);
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
