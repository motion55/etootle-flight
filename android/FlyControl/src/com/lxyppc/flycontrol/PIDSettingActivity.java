package com.lxyppc.flycontrol;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.InputType;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;
public class PIDSettingActivity extends Activity {
	public class PIDParam{
		PIDParam(Context ctx){
			p = new EditText(ctx);
			i = new EditText(ctx);
			d = new EditText(ctx);
			p.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			i.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
			d.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_FLAG_DECIMAL|InputType.TYPE_NUMBER_FLAG_SIGNED);
		}
		EditText p;
		EditText i;
		EditText d;
	}
	
	private PIDParam[] params = new PIDParam[3];
	private EditText[] edits = new EditText[9];
	private QuadParamSettingActivity.ParamButton buttons;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_pid_setting);
		LinearLayout tl = (LinearLayout)this.findViewById(R.id.pid_param_layout);
		String[] name = new String[]{
			getString(R.string.quad_pid_x),
			getString(R.string.quad_pid_y),
			getString(R.string.quad_pid_z),
		};
		
		{
			LinearLayout ll = new LinearLayout(this);
			LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
			lp.weight = 2;
			TextView v1 = new TextView(this);
			TextView v2 = new TextView(this);
			TextView v3 = new TextView(this);
			TextView v4 = new TextView(this);
			v1.setText(R.string.quad_pid_param);
			v2.setText(R.string.quad_pid_p);
			v3.setText(R.string.quad_pid_i);
			v4.setText(R.string.quad_pid_d);
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
			params[i] = new PIDParam(this);
			ll.addView(v1,lp);
			ll.addView(params[i].p,lp);
			ll.addView(params[i].i,lp);
			ll.addView(params[i].d,lp);
			tl.addView(ll);
			
			edits[i*3 + 0] = params[i].p;
			edits[i*3 + 1] = params[i].i;
			edits[i*3 + 2] = params[i].d;
		}
		buttons = new QuadParamSettingActivity.ParamButton(this, 19, 9, edits);
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
